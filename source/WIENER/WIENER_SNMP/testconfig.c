#include <testape.h>

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>

static FILE *m_taskFile = NULL;

static void openOutputFile(void)
{
	if (!m_taskFile)
		m_taskFile = fopen("testape.tasks", "w");
}

void testape_custom_error_int(char *file, int line, int actual, int expected, char *desc_actual, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected %d (%s), was %d. [%s]\n",
			file, line, desc_actual, expected, desc_expected, actual, testape_current_test);
}

void testape_custom_error_hex(char *file, int line, int actual, int expected, int mask, char *desc_actual, char *desc_expected)
{
	openOutputFile();

	if (mask == 0xff) {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected 0x%02x (%s), was 0x%02x.\n",
				file, line, desc_actual, expected & mask, desc_expected, actual & mask);
	} else if (mask == 0xffff) {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected 0x%04x (%s), was 0x%04x.\n",
				file, line, desc_actual, expected & mask, desc_expected, actual & mask);
	} else if (mask == 0xffffffff) {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected 0x%08x (%s), was 0x%08x.\n",
				file, line, desc_actual, expected & mask, desc_expected, actual & mask);
	} else {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected 0x%08x (mask 0x%08x), was 0x%08x.\n",
				file, line, desc_actual, expected, mask, actual);
	}
}

void testape_custom_error_str(char *file, int line, char *actual, char *expected, char *desc_actual, char *desc_expected)
{
	openOutputFile();

	if (strlen(expected) < 20 && strlen(actual) < 20) {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed contents of %s. Expected '%s' (%s), was '%s'.\n",
				file, line, desc_actual, expected, desc_expected, actual);
	} else {
		fprintf(m_taskFile, "%s\t%d\terr\tFailed contents of %s.\n",
				file, line, desc_actual);
	}
}

void testape_custom_error_mem(char *file, int line, int size, char *actual, char *expected, char *desc_actual, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed contents of %s.\n",
			file, line, desc_expected);
}

void testape_custom_error_mem_size(char *file, int line, int actual, int expected, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed length of %s. Expected %d, was %d.\n",
			file, line, desc_expected, expected, actual);
}

void testape_custom_error_file(char *file, int line, char *expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed existence of %s.\n",
			file, line, expected);
}

void testape_custom_error_file_size(char *file, int line, int actual, int expected, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed filesize of %s. Expected %d, was %d.\n",
			file, line, desc_expected, expected, actual);
}

void testape_custom_error_fp(char *file, int line, double actual, double expected, char *desc_actual, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed value of %s. Expected %f (%s), was %f.\n",
			file, line, desc_actual, expected, desc_expected, actual);
}

void testape_custom_error_function(char *file, int line, char *actual, char *expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed function call. Expected %s, was %s.\n",
			file, line, expected, actual);
}

void testape_custom_error_crash(char *file, int line, char *desc_expected)
{
	openOutputFile();

	fprintf(m_taskFile, "%s\t%d\terr\tFailed program fault. Expected %s\n",
			file, line, desc_expected);
}

void testape_custom_summary(int passed, int failed, int skipped, int errors)
{
	char *type = "";

	if (failed || errors)
		type = "err\t";
	else if (skipped)
		type = "warn\t";

	openOutputFile();

	fprintf(m_taskFile, "%sTest summary: %d passed, %d failed (%d errors), %d skipped\n",
			type, passed, failed, errors, skipped);
}
