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


static FILE *global_asm = NULL;
static isize global_asm_indent = 0;

void asm_printf(char const *fmt, ...) {
#define LINE_MAX 256
	char out_asm[LINE_MAX] = {0};
	isize i = 0;
	int written = 0;
	va_list args;

	if (fmt == NULL) {
		fwrite("\n", 1, 1, global_asm);
		return;
	}
	va_start(args, fmt);


	written = vsnprintf(out_asm, LINE_MAX, fmt, args);
	if (written >= LINE_MAX - 2) {
		fprintf(stderr, "LINE_MAX is not sufficient for instruction length\n");
	} else {
		if (out_asm[written-1] != '\n') {
			out_asm[written] = '\n';
		}
		for (i = 0; i < global_asm_indent; i++) {
			fwrite("\t", 1, 1, global_asm);
		}
		fwrite(out_asm, 1, written+1, global_asm);
	}

	va_end(args);
}

void write_asm_init(void) {
	isize i;
	char *lines[] = {
		"section .data",
		"title: db 'Win64', 0",
		"msg: db 'Hellope!', 0",
		"section .text",
		"",
		"extern ExitProcess",
		"extern MessageBoxA",
		""
	};
	for (i = 0; i < count_of(lines); i++) {
		asm_printf(lines[i]);
	}
}

void asm_proc_start(char const *name) {
	asm_printf("");
	asm_printf("global %s", name);
	asm_printf("%s:", name);
	global_asm_indent++;
}

void asm_proc_end(void) {
	// asm_printf("pop rbp");
	asm_printf("ret");
	global_asm_indent--;
}

void write_asm(void) {
	asm_proc_start("WinMain");
	asm_printf("sub rsp, 28h");
	asm_printf("mov rcx, 0");       // HWND_DESKTOP
	asm_printf("lea rdx, [msg]");   // lpText
	asm_printf("lea r8, [title]");  // lpCaption
	asm_printf("mov r9d, 0");       // MB_OK
	asm_printf("call MessageBoxA");
	asm_printf("add rsp, 28h");

	asm_printf("mov rcx, rax");
	asm_printf("call ExitProcess");
	asm_proc_end();

}


int main(int argc, char **argv) {
	bool ok = false;
	char *assembler[] = {
		"C:\\Program Files\\NASM\\nasm.exe",
		"src\\foo.asm",
		"-f win64",
		"-F cv8",
		""
	};
	char *linker[] = {
		"link.exe",
		"src\\foo.obj",
		"/OUT:foo.exe",
		"/PDB:foo.pdb",
		"/DEBUG",
		"/NOLOGO",
		"/SUBSYSTEM:WINDOWS",
		"/LARGEADDRESSAWARE:NO",
		"/ENTRY:WinMain",
		"kernel32.lib",
		"user32.lib",
		""
	};

	global_asm = fopen("src\\foo.asm", "wb");
	if (!global_asm) {
		return 1;
	}
	write_asm_init();
	write_asm();
	fclose(global_asm);

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
