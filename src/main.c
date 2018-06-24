#include "common.c"




void win32_print_error(int err) {
	char *msg = 0;
	char buf[MAX_PATH] = {0};

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	               FORMAT_MESSAGE_FROM_SYSTEM |
	               FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL,
	               err,
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	               (LPSTR)&msg,
	               0, NULL);
	snprintf(buf, count_of(buf), "Error: %s", msg);
	fprintf(stderr, "%s\n", buf);
	LocalFree(msg);
}

bool platform_create_process(char **args, isize num_args) {
	bool result = false;
	if (num_args > 0) {
		isize i;
		char cmd_line[1024] = {0};
		STARTUPINFO info = {0};
		PROCESS_INFORMATION proc_info = {0};

		info.cb = sizeof(STARTUPINFO);
		info.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
		info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		info.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		info.dwFlags = STARTF_USESTDHANDLES;

		for (i = 0; i < num_args; i++) {
			strncat(cmd_line, args[i], count_of(cmd_line));
			strncat(cmd_line, " ", count_of(cmd_line));
		}

		if (!CreateProcessA(NULL, cmd_line, NULL, NULL, true, 0, NULL, NULL, &info, &proc_info)) {
			int err = GetLastError();
			win32_print_error(err);
			fprintf(stderr, "%s\n", cmd_line);
		} else {
			DWORD wait_res = WaitForSingleObject(proc_info.hProcess, INFINITE);
			if (wait_res == WAIT_OBJECT_0) {
				DWORD exit_code = 0;
				GetExitCodeProcess(proc_info.hProcess, &exit_code);
				if (exit_code != 0) {
					fprintf(stderr, "Process failed: %ld\n", exit_code);
				} else {
					result = true;
				}
			} else {
				fprintf(stderr, "WaitForSingleObject failed: %ld\n", wait_res);
			}

			CloseHandle(proc_info.hProcess);
			CloseHandle(proc_info.hThread);
		}
	}
	return result;
}


int main(int argc, char **argv) {
	bool ok = false;
	char *assembler[] = {
		"C:\\Program Files\\NASM\\nasm.exe",
		"src\\test.asm",
		"-f win64",
		"-F cv8"
	};
	char *linker[] = {
		"link.exe",
		"src\\test.obj",
		"/OUT:test.exe",
		"/PDB:test.pdb",
		"/DEBUG",
		"/NOLOGO",
		"/SUBSYSTEM:CONSOLE", "kernel32.lib"
	};

	ok = platform_create_process(assembler, count_of(assembler));
	if (!ok) {
		return 1;
	}
	ok = platform_create_process(linker, count_of(linker));
	if (!ok) {
		return 1;
	}
	return 0;
}
