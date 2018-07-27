/* : : generated by conf from comp/conf.tab : : */

/*
 * supplemental <limits.h> values
 */

printf("#ifndef AIO_LISTIO_MAX\n");
printf("#define AIO_LISTIO_MAX 2\n");
printf("#endif\n");
printf("#ifndef AIO_MAX\n");
printf("#define AIO_MAX 1\n");
printf("#endif\n");
printf("#ifndef ARG_MAX\n");
printf("#ifndef NCARGS\n");
printf("#define NCARGS %ld\n", ( long )(NCARGS));
printf("#endif\n");
printf("#define ARG_MAX NCARGS\n");
printf("#endif\n");
printf("#ifndef ATEXIT_MAX\n");
printf("#define ATEXIT_MAX 2147483647\n");
printf("#endif\n");
printf("#ifndef CHAR_BIT\n");
printf("#define CHAR_BIT 8\n");
printf("#endif\n");
printf("#ifndef CHAR_MAX\n");
printf("#define CHAR_MAX 127\n");
printf("#endif\n");
printf("#ifndef CHAR_MIN\n");
printf("#define CHAR_MIN -128\n");
printf("#endif\n");
printf("#ifndef CHILD_MAX\n");
printf("#define CHILD_MAX 62754\n");
printf("#endif\n");
printf("#ifndef CLK_TCK\n");
printf("#ifndef HZ\n");
printf("#define HZ %ld\n", ( long )(HZ));
printf("#endif\n");
printf("#define CLK_TCK HZ\n");
printf("#endif\n");
printf("#ifndef FCHR_MAX\n");
printf("#ifndef LONG_MAX\n");
printf("#define LONG_MAX %ld\n", ( long )(LONG_MAX));
printf("#endif\n");
printf("#define FCHR_MAX LONG_MAX\n");
printf("#endif\n");
printf("#ifndef FILESIZEBITS\n");
printf("#define FILESIZEBITS 32\n");
printf("#endif\n");
printf("#ifndef INT_MIN\n");
printf("#define INT_MIN -2147483648\n");
printf("#endif\n");
printf("#ifndef LLONG_MAX\n");
printf("#define LLONG_MAX 9223372036854775807\n");
printf("#endif\n");
printf("#ifndef LLONG_MIN\n");
printf("#define LLONG_MIN -9223372036854775808\n");
printf("#endif\n");
printf("#ifndef LONG_MAX\n");
printf("#define LONG_MAX 9223372036854775807\n");
printf("#endif\n");
printf("#ifndef LONG_MIN\n");
printf("#define LONG_MIN -9223372036854775808\n");
printf("#endif\n");
printf("#ifndef MB_LEN_MAX\n");
printf("#define MB_LEN_MAX 16\n");
printf("#endif\n");
printf("#ifndef NSS_BUFLEN_GROUP\n");
printf("#define NSS_BUFLEN_GROUP 1024\n");
printf("#endif\n");
printf("#ifndef NSS_BUFLEN_PASSWD\n");
printf("#define NSS_BUFLEN_PASSWD 1024\n");
printf("#endif\n");
printf("#ifndef OPEN_MAX\n");
printf("#define OPEN_MAX 16384\n");
printf("#endif\n");
printf("#ifndef PASS_MAX\n");
printf("#define PASS_MAX 8192\n");
printf("#endif\n");
printf("#ifndef PID_MAX\n");
printf("#define PID_MAX 32768\n");
printf("#endif\n");
printf("#ifndef PTRDIFF_MAX\n");
printf("#define PTRDIFF_MAX 9223372036854775807\n");
printf("#endif\n");
printf("#ifndef PTRDIFF_MIN\n");
printf("#define PTRDIFF_MIN -9223372036854775808\n");
printf("#endif\n");
printf("#ifndef SCHAR_MAX\n");
printf("#define SCHAR_MAX 127\n");
printf("#endif\n");
printf("#ifndef SCHAR_MIN\n");
printf("#define SCHAR_MIN -128\n");
printf("#endif\n");
printf("#ifndef SEM_NSEMS_MAX\n");
printf("#define SEM_NSEMS_MAX 256\n");
printf("#endif\n");
printf("#ifndef SHRT_MIN\n");
printf("#define SHRT_MIN -32768\n");
printf("#endif\n");
printf("#ifndef SIGQUEUE_MAX\n");
printf("#define SIGQUEUE_MAX 62754\n");
printf("#endif\n");
printf("#ifndef SIG_ATOMIC_MAX\n");
printf("#define SIG_ATOMIC_MAX 2147483647\n");
printf("#endif\n");
printf("#ifndef SIG_ATOMIC_MIN\n");
printf("#define SIG_ATOMIC_MIN -2147483648\n");
printf("#endif\n");
printf("#ifndef SIZE_MAX\n");
printf("#ifndef UINT_MAX\n");
printf("#define UINT_MAX %lu\n", ( unsigned long )(UINT_MAX));
printf("#endif\n");
printf("#define SIZE_MAX UINT_MAX\n");
printf("#endif\n");
printf("#ifndef SSIZE_MAX\n");
printf("#ifndef INT_MAX\n");
printf("#define INT_MAX %ld\n", ( long )(INT_MAX));
printf("#endif\n");
printf("#define SSIZE_MAX INT_MAX\n");
printf("#endif\n");
printf("#ifndef STD_BLK\n");
printf("#define STD_BLK 1024\n");
printf("#endif\n");
printf("#ifndef SYSPID_MAX\n");
printf("#define SYSPID_MAX 2\n");
printf("#endif\n");
printf("#ifndef THREAD_DESTRUCTOR_ITERATIONS\n");
printf("#ifndef PTHREAD_DESTRUCTOR_ITERATIONS\n");
printf("#define PTHREAD_DESTRUCTOR_ITERATIONS %ld\n",
       ( long )(PTHREAD_DESTRUCTOR_ITERATIONS));
printf("#endif\n");
printf(
"#define THREAD_DESTRUCTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS\n");
printf("#endif\n");
printf("#ifndef THREAD_KEYS_MAX\n");
printf("#ifndef PTHREAD_KEYS_MAX\n");
printf("#define PTHREAD_KEYS_MAX %ld\n", ( long )(PTHREAD_KEYS_MAX));
printf("#endif\n");
printf("#define THREAD_KEYS_MAX PTHREAD_KEYS_MAX\n");
printf("#endif\n");
printf("#ifndef THREAD_STACK_MIN\n");
printf("#ifndef PTHREAD_STACK_MIN\n");
printf("#define PTHREAD_STACK_MIN %ld\n", ( long )(PTHREAD_STACK_MIN));
printf("#endif\n");
printf("#define THREAD_STACK_MIN PTHREAD_STACK_MIN\n");
printf("#endif\n");
printf("#ifndef THREAD_THREADS_MAX\n");
printf("#define THREAD_THREADS_MAX 64\n");
printf("#endif\n");
printf("#ifndef TMP_MAX\n");
printf("#define TMP_MAX 238328\n");
printf("#endif\n");
printf("#ifndef UCHAR_MAX\n");
printf("#define UCHAR_MAX 255\n");
printf("#endif\n");
printf("#ifndef UID_MAX\n");
printf("#define UID_MAX 60002\n");
printf("#endif\n");
printf("#ifndef ULLONG_MAX\n");
printf("#define ULLONG_MAX 18446744073709551615\n");
printf("#endif\n");
printf("#ifndef ULONG_MAX\n");
printf("#define ULONG_MAX 18446744073709551615\n");
printf("#endif\n");
printf("#ifndef USHRT_MAX\n");
printf("#define USHRT_MAX 65535\n");
printf("#endif\n");
printf("#ifndef WCHAR_MAX\n");
printf("#define WCHAR_MAX 2147483647\n");
printf("#endif\n");
printf("#ifndef WCHAR_MIN\n");
printf("#define WCHAR_MIN -2147483648\n");
printf("#endif\n");
