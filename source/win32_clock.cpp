#include <intrin.h>

#pragma intrinsic(__rdtsc)

#define SECONDS_TO_MILLI(x) ((x)*1000ULL)
#define SECONDS_TO_MICRO(x) ((x)*1000000ULL)
#define SECONDS_TO_NANO(x)  ((x)*1000000000ULL)
#define MILLI_TO_SECONDS(x) ((x)/1000ULL)
#define MILLI_TO_MICRO(x)   ((x)*1000ULL)
#define MILLI_TO_NANO(x)    ((x)*1000000ULL)
#define MICRO_TO_SECONDS(x) ((x)/1000000ULL)
#define MICRO_TO_MILLI(x)   ((x)/1000ULL)
#define MICRO_TO_NANO(x)    ((x)*1000ULL)
#define NANO_TO_SECONDS(x)  ((x)/1000000000ULL)
#define NANO_TO_MILLI(x)    ((x)/1000000ULL)
#define NANO_TO_MICRO(x)    ((x)/1000ULL)


static u64 clock_frequency;
static u32 minimum_clock_granularity_in_ms;
static u32 maximum_clock_granularity_in_ms;

struct NanoClock
{
	u64 last_time;

	NanoClock()
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		last_time = time.QuadPart;
	}
};


void SetHighPrecisionMode()
{
	WIN32_ASSERT(minimum_clock_granularity_in_ms != 0, "Minimum time granularity not set. Make sure to call 'InitializeTimeModule'\n");

	if (timeBeginPeriod(minimum_clock_granularity_in_ms) != TIMERR_NOERROR)
		WIN32_REPORT_ERROR("Couldn't set time granularity to %d ms\n", minimum_clock_granularity_in_ms);
}

void SetLowPrecisionMode()
{
	WIN32_ASSERT(minimum_clock_granularity_in_ms != 0, "Minimum time granularity not set. Make sure to call 'InitializeTimeModule'\n");

	if (timeEndPeriod(minimum_clock_granularity_in_ms) != TIMERR_NOERROR)
		WIN32_REPORT_ERROR("Couldn't reset time granularity from %d ms\n", minimum_clock_granularity_in_ms);
}

void InitializeTimeModule()
{
	TIMECAPS time_info;
	WIN32_ASSERT(timeGetDevCaps(&time_info, sizeof(time_info)) == MMSYSERR_NOERROR, "Couldn't get granularity info.\n");

	minimum_clock_granularity_in_ms = time_info.wPeriodMin;
	maximum_clock_granularity_in_ms = time_info.wPeriodMax;

	// Set clock granularity.
	SetHighPrecisionMode();

	// Query clock frequency.
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	clock_frequency = frequency.QuadPart;
}


u64 Tick(NanoClock& clock, u64 cap, u64 allowed_variance = 100)
{
	// NTDelay - https://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FThread%2FNtDelayExecution.html
	// KeDelayExecutionThread - https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/wdm/nf-wdm-kedelayexecutionthread

	WIN32_ASSERT(clock_frequency != 0, "Clock frequency not set. Make sure to call 'InitializeTimeModule'\n");
	WIN32_ASSERT(cap < SECONDS_TO_NANO(60), "Cannot sleep more than a minute. Cap was %llu ns.\n", cap);
	WIN32_ASSERT(cap > MILLI_TO_NANO(minimum_clock_granularity_in_ms), "Cap (%llu ns) cannot be lower than granularity (%llu ns).\n", cap, MILLI_TO_NANO(minimum_clock_granularity_in_ms));

	LARGE_INTEGER current_temp;
	QueryPerformanceCounter(&current_temp);
	u64 current = current_temp.QuadPart;

	u64 duration = RoundToU64(SECONDS_TO_NANO(cast(current - clock.last_time, f32) / clock_frequency));

	// Sleep if program runs faster than cap.
	while (duration < cap - MILLI_TO_NANO(minimum_clock_granularity_in_ms))
	{
		Sleep(cast(NANO_TO_MILLI(cap - duration) - minimum_clock_granularity_in_ms, u32));

		QueryPerformanceCounter(&current_temp);
		current  = current_temp.QuadPart;
		duration = RoundToU64(SECONDS_TO_NANO(cast(current - clock.last_time, f32) / clock_frequency));
	}

	// Spin lock to get accurate time.
	while (duration < cap - allowed_variance)
	{
		QueryPerformanceCounter(&current_temp);
		current  = current_temp.QuadPart;
		duration = RoundToU64(SECONDS_TO_NANO(cast(current - clock.last_time, f32) / clock_frequency));
	}

	clock.last_time = current;
	return duration;
}

u64 Tick(NanoClock& clock)
{
	// NTDelay - https://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FThread%2FNtDelayExecution.html
	// KeDelayExecutionThread - https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/wdm/nf-wdm-kedelayexecutionthread

	WIN32_ASSERT(clock_frequency != 0, "Clock frequency not set. Make sure to call 'InitializeTimeModule'\n");

	LARGE_INTEGER current_temp;
	QueryPerformanceCounter(&current_temp);
	u64 current = current_temp.QuadPart;

	u64 duration = RoundToU64(SECONDS_TO_NANO(cast(current - clock.last_time, f32) / clock_frequency));

	clock.last_time = current;
	return duration;
}

bool Timer(NanoClock& clock, u64 time)
{
	WIN32_ASSERT(clock_frequency != 0, "Clock frequency not set. Make sure to call 'InitializeTimeModule'\n");

	LARGE_INTEGER current_temp;
	QueryPerformanceCounter(&current_temp);
	u64 current = current_temp.QuadPart;

	u64 duration = RoundToU64(SECONDS_TO_NANO(cast(current - clock.last_time, f32) / clock_frequency));

	if (duration >= time)
	{
		clock.last_time = current;
		return true;
	}
	return false;
}

u64 CycleCount()
{
	return __rdtsc();
}