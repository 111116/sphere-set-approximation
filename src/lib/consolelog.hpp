// console.log v1.1
// Javascript-styled console logger for C++
// https://github.com/111116/console.log
#pragma once

#include <iostream>
#include <unistd.h> // isatty
#include <unordered_map> // label-time pairs
#include <string> // labels
#include <chrono> // time
#include <cmath>
#include <mutex>

namespace github111116 {
class ConsoleLogger
{
protected:
	std::ostream& out;

public:
	std::string  infoColor = "\033[0;32m";
	std::string  warnColor = "\033[1;33m";
	std::string errorColor = "\033[1;31m";
	bool colored = false;
	int loglevel = 0;

	ConsoleLogger(std::ostream& out, bool colored);
	// basic logging functions
	template <typename... Args> void log(const Args&... args); // loglevel:0
	template <typename... Args> void info(const Args&... args); // loglevel:1
	template <typename... Args> void warn(const Args&... args); // loglevel:2
	template <typename... Args> void error(const Args&... args); // loglevel:3
	template <typename... Args> void debug(const Args&... args); // only affected by NDEBUG flag
	// timer functions
	void time(const std::string& label = "default");
	void timeLog(const std::string& label = "default");
	void timeEnd(const std::string& label = "default");
	// counter functions
	long long count(const std::string& label = "default");
	long long countReset(const std::string& label = "default");

protected:
	void print();
	template <typename T>
	void print(const T& t);
	template <typename T, typename... Args>
	void print(const T& t, const Args&... args);
	std::unordered_map<std::string, std::chrono::time_point<std::chrono::system_clock>> starttimes;
	std::unordered_map<std::string, long long> counts;
	std::mutex mtx;
};
}


// defining global variable "console"
__attribute__((weak)) github111116::ConsoleLogger console(std::cerr, isatty(fileno(stderr)));



// ========== Implementation Starts ===========

namespace github111116 {

inline ConsoleLogger::ConsoleLogger(std::ostream& out, bool colored):
	out(out), colored(colored)
{
}

template <typename... Args>
void ConsoleLogger::log(const Args&... args)
{
	if (loglevel > 0) return;
	mtx.lock();
	out << "    ";
	print(args...);
	mtx.unlock();
}

template <typename... Args>
void ConsoleLogger::debug(const Args&... args)
{
#ifndef NDEBUG
	mtx.lock();
	out << "... ";
	print(args...);
	mtx.unlock();
#endif
}

template <typename... Args>
void ConsoleLogger::info(const Args&... args)
{
	if (loglevel > 1) return;
	mtx.lock();
	if (colored) out << infoColor;
	out << "[i] ";
	if (colored) out << "\033[0m";
	print(args...);
	mtx.unlock();
}

template <typename... Args>
void ConsoleLogger::warn(const Args&... args)
{
	if (loglevel > 2) return;
	mtx.lock();
	if (colored) out << warnColor;
	out << "[!] ";
	print(args...);
	if (colored) out << "\033[0m";
	mtx.unlock();
}

template <typename... Args>
void ConsoleLogger::error(const Args&... args)
{
	if (loglevel > 3) return;
	mtx.lock();
	if (colored) out << errorColor;
	out << "[X] ";
	print(args...);
	if (colored) out << "\033[0m";
	mtx.unlock();
}


inline void ConsoleLogger::print()
{
	out << std::endl;
}

template <typename T>
void ConsoleLogger::print(const T& t)
{
	out << t;
	print();
}

template <typename T, typename... Args>
void ConsoleLogger::print(const T& t, const Args&... args)
{
	out << t << " ";
	print(args...);
}

inline void ConsoleLogger::time(const std::string& label)
{
	if (starttimes.count(label))
	{
		mtx.lock();
		warn("Timer \"" + label + "\" already exists.");
		mtx.unlock();
		return;
	}
	mtx.lock();
	starttimes[label] = std::chrono::system_clock::now();
	mtx.unlock();
}

inline void ConsoleLogger::timeLog(const std::string& label)
{
	if (!starttimes.count(label))
	{
		mtx.lock();
		warn("Timer \"" + label + "\" doesn't exist.");
		mtx.unlock();
		return;
	}
	mtx.lock();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> seconds = now - starttimes[label];
	if (loglevel <= 0)
		out << "    "+label+": " << (long long)(seconds.count()*1000)/1000 << "s" << std::endl;
	mtx.unlock();
}

inline void ConsoleLogger::timeEnd(const std::string& label)
{
	if (!starttimes.count(label))
	{
		mtx.lock();
		warn("Timer \"" + label + "\" doesn't exist.");
		mtx.unlock();
		return;
	}
	mtx.lock();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> seconds = now - starttimes[label];
	if (loglevel <= 0)
		out << "    "+label+": " << 0.001*round(seconds.count()*1000) << "s - timer ended" << std::endl;
	starttimes.erase(label);
	mtx.unlock();
}

inline long long ConsoleLogger::countReset(const std::string& label)
{
	if (!counts.count(label))
	{
		mtx.lock();
		warn("Counter \"" + label + "\" doesn't exist.");
		mtx.unlock();
		return -1;
	}
	mtx.lock();
	counts[label] = 0;
	if (loglevel <= 0)
		out << "    "+label+": 0" << std::endl;
	mtx.unlock();
	return 0;
}

inline long long ConsoleLogger::count(const std::string& label)
{
	mtx.lock();
	int t = ++counts[label];
	if (loglevel <= 0)
		out << "    "+label+": " << t << std::endl;
	mtx.unlock();
	return t;
}

} // end namespace
