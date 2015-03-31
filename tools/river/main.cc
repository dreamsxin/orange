#include <iostream>
#include <helper/args.h>
#include <helper/link.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
using namespace boost::filesystem;
using namespace boost;

void testFiles(std::vector<path> paths);

int main(int argc, char **argv) {
	cOptions options("River version 0.1"); 

	cCommandOption version({"v", "version"}, "Prints version", false);
	options.mainState.add(&version);

	cOptionsState test("test", "Tests your orange project");

	cCommandOption testFailed({"f", "failed"}, "Only test tests that failed on the last run", false);
	test.add(&testFailed);

	options.mainState.addState(&test); 

	options.parse(argc, argv);

	if (version.isSet()) {
		std::cout << "River version 0.1" << std::endl;
		exit(0);
	}

	for (std::string option : test.unparsed()) {
	}

	if (test.isActive() && test.unparsed().size() == 0) {
		// for now, test everything in test, going through each subdirectory.
		path p("test");

		regex filter(".*\\.or");
		std::vector<path> toTest;
		
		if (!exists(p)) {
			exit(0); 
		} else {
			for (auto& entry : make_iterator_range(recursive_directory_iterator(p), {})) {
				smatch what;
				if (!regex_match(entry.path().filename().string(), what, filter)) continue;
				toTest.push_back(entry.path());
			}

			testFiles(toTest);
		}
	} else if (test.isActive() && test.unparsed().size() > 0) {
		regex filter(".*\\.or");
		std::vector<path> toTest;

		for (std::string subdir : test.unparsed()) {
			std::string search = "test/" + subdir; 
			path p(search);

			if (!exists(p)) {
				std::cerr << "fatal: path " << search << " does not exist.\n";
				exit(1);
			}	

			for (auto& entry : make_iterator_range(recursive_directory_iterator(p), {})) {
				smatch what;
				if (!regex_match(entry.path().filename().string(), what, filter)) continue;
				toTest.push_back(entry.path());
			}
		
		}

		testFiles(toTest);
	}

	return 0;
}

void testFiles(std::vector<path> paths) {
  boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();

	int total = 0;
	int failed = 0;
	int passed = 0;
	int warnings = 0;

	std::vector<std::string> errors;

	const char *ocPath = programPath("oc");
	if (ocPath == nullptr) {
		std::cerr << "fatal: could not find oc in $PATH.\n";
		exit(1);
	}

	try {
		boost::filesystem::create_directories("build/test");	
	} catch (filesystem_error e) {
		std::cerr << "fatal: could not build directory build/test\n"; 
		exit(1);
	}

	for (path p : paths) {
		std::vector<const char *> options;
		options.push_back(p.string().c_str());
		options.push_back("-o");

		std::string output = p.stem().string();
		output = "build/test/" + output;

#ifdef _WIN32 
		output += ".exe";
#endif 

		options.push_back(output.c_str());

		int status = invokeProgramWithOptions(ocPath, options, true);
		bool added_error = false;

		path output_p(output);

		if (!exists(output_p)) status = 1;

		if (status && !added_error) {
			errors.push_back(p.string() + " (compile failed)");
			added_error = true;
		}

		if (status == 0) {
			std::vector<const char *> roptions;
			status = invokeProgramWithOptions(output.c_str(), roptions, true);
		}

		if (status && !added_error) {
			errors.push_back(p.string() + " (program did not return 0)");
			added_error = true;
		}

		if (status == 0) {
			std::cout << "." << std::flush;
			passed++;
		} else {
			std::cout << "F" << std::flush;
			failed++;
		}

		if (exists(output_p)) {
			remove(output_p);		
		}

		total++;

		if ((total % 40) == 0) std::cout << "\n";
	}

	boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration diff = endTime - startTime;
  long ms = diff.total_milliseconds();

	std::cout << "\n\nTest results (" << (float)ms/1000.0f << " seconds):\n";
	std::cout << "\t" << total << " tests total.\n";
	std::cout << "\t" << passed << " tests passed.\n";
	std::cout << "\t" << failed << " tests failed.\n";

	if (errors.size() > 0) {
		std::cout << "\nErrors:\n";
		for (std::string error : errors) {
			std::cout << "\t" << error << std::endl;
		}
		std::cout << std::endl;
	}
}
