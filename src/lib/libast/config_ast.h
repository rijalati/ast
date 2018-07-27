// These are all the symbols defined by the legacy Nmake/iffe based build
// system I could identify. Some of them may no longer be relevant because we
// have pruned a lot of the code that is not relevant to building the `ksh`
// and related binaries and libraries. Nonetheless, those symbols are included
// here just in case we've overlooked something in the transition to the Meson
// build tool.
//
// I am explicitly including all such symbols to help us identify areas where
// we may be missing a platform feature test. Such symbols will show up as
// `#undef` comments after the Meson config step has been run to produce the
// `build/config_ast.h` header. There will be a lot of false positives until
// we have removed symbols that are known to no longer be relevant.
//
// TODO: At some point in the future (e.g., 2019-01-01) irrelevant items in
// the following list can be removed and the preceding paragraph removed.

// The following symbols are in lexical order within a given prefix; e.g.,
// `_lib_`. But the prefix groups are not in lexical order. The esoteric
// prefix groups are listed first. The standard prefix symbols (e.g., `_sys_`,
// `_hdr_`, etc.) based on tests like `cc.has_function()` or `cc.has_header()`
// are enumerated later.

#define _GNU_SOURCE 1

// These are atypical platform symbols with respect to how they are
// determined. They are either hardcoded or not determined via the usual
// platform feature detection and naming conventions.
#define MESON_BUILD 1
#define HOSTTYPE "linux.i386-64"
#define BASH_MACHTYPE "x86_64-unknown-linux"
#define STAT_ST_MTIM 1
/* #undef STAT_ST_MTIMESPEC */
#define CONF_LIBPATH "lib:LD_LIBRARY_PATH"
#define _API_ast 20130625
#define _PACKAGE_ast 1
#define _PROC_PROG "/proc/self/exe"
/* #undef _DARWIN_C_SOURCE */
#define ALIGN_BOUND ALIGN_BOUND2
#define ALIGN_BOUND1 ALIGN_BOUND2
#define ALIGN_BOUND2 16

#define _ast_sizeof_int 4
#define _ast_sizeof_long 8
#define _ast_sizeof_off_t 8
#define _ast_sizeof_pointer 8
#define _ast_sizeof_size_t 8
#define _cmd_newgrp 1
/* #undef _cmd_nfsd */
#define _cmd_tput 1
/* #undef _cmd_universe */
#define _dll_DYNAMIC 0
#define _DLL_NEXT_PATH "/usr/lib/libc.so.6"
#define _locale_utf8_str "UTF-8"
/* #undef _map_libc */
#define _map_malloc 1
/* #undef _map_spawnve */
/* #undef _may_use_threads */
#define _more_long_int 1
#define _more_void_int 1
#define _pth_ed "/opt/ast/bin/ed"
/* #undef _pipe_rw */
/* #undef _proto_bcopy */
/* #undef _proto_bzero */
#define _ptr_bits 64
/* #undef _pty_clone */
#define _pty_first "/dev/ptyp0"
#define _socketpair_shutdown_mode 1
/* #undef _std_remove */
/* #undef _std_signal */
#define _stk_down 1
/* #undef isnanl */

// What follows are all the symbols that should be based on Meson feature
// tests like `cc.has_header()` and `cc.find_library()`.
#define _hdr_arpa_inet 1
#define _hdr_dl 0
#define _hdr_dlfcn 1
#define _hdr_dll 0
/* #undef _hdr_execargs */
#define _hdr_execinfo 1
/* #undef _hdr_filio */
/* #undef _hdr_float */
/* #undef _hdr_fsg */
/* #undef _hdr_jioctl */
#define _hdr_langinfo 1
/* #undef _hdr_locale */
#define _hdr_mach_o_dyld 0
#define _hdr_malloc 1
/* #undef _hdr_math */
/* #undef _hdr_mman */
/* #undef _hdr_nc */
/* #undef _hdr_netdb */
#define _hdr_netinet_in 1
/* #undef _hdr_netinet_tcp */
#define _hdr_nl_types 1
#define _hdr_paths 1
#define _hdr_pty 1
#define _hdr_rld_interface 0
/* #undef _hdr_stat */
/* #undef _hdr_stddef */
/* #undef _hdr_stdlib */
#define _hdr_sys_sysmacros 1
/* #undef _hdr_termio */
#define _hdr_termios 1
/* #undef _hdr_unistd */
#define _hdr_util 0
#define _hdr_utmp 1
#define _hdr_utmpx 1
/* #undef _hdr_values */
/* #undef _hdr_wchar */
/* #undef _hdr_wctype */

#define _lib___libc_malloc 0
/* #undef _lib___malloc */
/* #undef _lib__getpty */
/* #undef _lib_alloca */
#define _lib_clock_gettime 1
/* #undef _lib_close64 */
/* #undef _lib_confstr */
/* #undef _lib_creat64 */
#define _lib_dllload 0
#define _lib_dlopen 1
/* #undef _lib_fdopendir */
#define _lib_fsid 0
/* #undef _lib_fstat64 */
/* #undef _lib_fstatvfs64 */
/* #undef _lib_fsync */
/* #undef _lib_ftruncate */
/* #undef _lib_ftruncate64 */
#define _lib_getdomainname 1
/* #undef _lib_getexecname */
#define _lib_getfsgid 0
#define _lib_getgrent 1
#define _lib_gethostid 1
#define _lib_gethostname 1
/* #undef _lib_getsid */
#define _lib_getsockname 0
#define _lib_getsockopt 0
#define _lib_gettimeofday 1
#define _lib_grantpt 1
/* #undef _lib_htonl */
/* #undef _lib_htons */
#define _lib_inet_ntoa 0
#define _lib_isfsg 0
/* #undef _lib_iswalnum */
/* #undef _lib_iswctype */
/* #undef _lib_iswprint */
/* #undef _lib_lchmod */
/* #undef _lib_lchown */
#define _lib_loadbind 0
/* #undef _lib_lseek64 */
/* #undef _lib_lstat */
/* #undef _lib_lstat64 */
#define _lib_mallinfo 1
#define _lib_mallopt 1
/* #undef _lib_memcntl */
/* #undef _lib_memdup */
/* #undef _lib_memzero */
/* #undef _lib_mmap64 */
#define _lib_mstats 0
/* #undef _lib_munmap64 */
/* #undef _lib_nl_langinfo */
/* #undef _lib_ntohl */
/* #undef _lib_ntohs */
/* #undef _lib_onexit */
/* #undef _lib_open64 */
/* #undef _lib_opendir */
#define _lib_openpty 1
/* #undef _lib_pathnative */
/* #undef _lib_pathposix */
#define _lib_pipe2 1
/* #undef _lib_poll */
/* #undef _lib_poll_fd_2 */
/* #undef _lib_poll_notimer */
#define _lib_posix_openpt 1
#define _lib_posix_spawn 1
/* #undef _lib_posix_spawnattr_setfchdir */
/* #undef _lib_posix_spawnattr_setsid */
/* #undef _lib_posix_spawnattr_setumask */
/* #undef _lib_pstat */
#define _lib_ptsname 1
#define _lib_pvalloc 1
/* #undef _lib_rewinddir */
/* #undef _lib_setlocale */
/* #undef _lib_setregid */
/* #undef _lib_setreuid */
/* #undef _lib_sigblock */
/* #undef _lib_sigflag */
/* #undef _lib_sigprocmask */
#define _lib_sigqueue 1
/* #undef _lib_sigsetmask */
/* #undef _lib_sigunblock */
/* #undef _lib_sigvec */
/* #undef _lib_socket */
/* #undef _lib_socketpair */
/* #undef _lib_spawn */
/* #undef _lib_spawn_mode */
/* #undef _lib_spawnve */
/* #undef _lib_spawnvex */
/* #undef _lib_statvfs */
/* #undef _lib_statvfs64 */
/* #undef _lib_stracmp */
/* #undef _lib_strlcat */
/* #undef _lib_strlcpy */
/* #undef _lib_strnacmp */
/* #undef _lib_strsignal */
/* #undef _lib_sync */
#define _lib_syncfs 1
#define _lib_sysinfo 0
#define _lib_tcgetattr 1
/* #undef _lib_tcgetpgrp */
/* #undef _lib_towlower */
/* #undef _lib_towupper */
/* #undef _lib_truncate64 */
#define _lib_uname 1
/* #undef _lib_universe */
/* #undef _lib_unlock */
#define _lib_unlockpt 1
/* #undef _lib_utime */
/* #undef _lib_utime_now */
#define _lib_utimensat 1
/* #undef _lib_wcrtomb */
/* #undef _lib_wcscmp */
/* #undef _lib_wcsxfrm */
/* #undef _lib_wcscpy */
/* #undef _lib_wctomb */

#define _mem_arena_mallinfo 1
/* #undef _mem_base_rel_utsname */
/* #undef _mem_bytes_total_mstats */
/* #undef _mem_c_line_termios */
/* #undef _mem_credit_sign_lconv */
/* #undef _mem_currency_symbol_lconv */
/* #undef _mem_d_fileno_dirent */
/* #undef _mem_d_ino_dirent */
/* #undef _mem_d_namlen_dirent */
/* #undef _mem_d_off_dirent */
/* #undef _mem_d_reclen_direct */
/* #undef _mem_d_reclen_dirent */
/* #undef _mem_d_type_dirent */
/* #undef _mem_debit_sign_lconv */
/* #undef _mem_decimal_point_lconv */
/* #undef _mem_e_xsize_eproc */
/* #undef _mem_f_basetype_statvfs */
/* #undef _mem_f_fsid_statvfs */
/* #undef _mem_f_fstr_statvfs */
/* #undef _mem_frac_digits_lconv */
/* #undef _mem_grouping_lconv */
/* #undef _mem_idnumber_utsname */
/* #undef _mem_int_curr_symbol_lconv */
/* #undef _mem_int_frac_digits_lconv */
/* #undef _mem_left_parenthesis_lconv */
/* #undef _mem_m_type_utsname */
#define _mem_mmap_anon 1
/* #undef _mem_mon_decimal_point_lconv */
/* #undef _mem_mon_grouping_lconv */
/* #undef _mem_mon_thousands_sep_lconv */
/* #undef _mem_msg_control_msghdr */
/* #undef _mem_n_cs_precedes_lconv */
/* #undef _mem_n_sep_by_space_lconv */
/* #undef _mem_n_sign_posn_lconv */
/* #undef _mem_negative_sign_lconv */
/* #undef _mem_nodeext_utsname */
/* #undef _mem_p_addr_extern_proc */
/* #undef _mem_p_cs_precedes_lconv */
/* #undef _mem_p_pid_extern_proc */
/* #undef _mem_p_sep_by_space_lconv */
/* #undef _mem_p_sign_posn_lconv */
/* #undef _mem_p_starttime_extern_proc */
/* #undef _mem_p_usrpri_extern_proc */
/* #undef _mem_p_wchan_extern_proc */
/* #undef _mem_pgroup_inheritance */
/* #undef _mem_pi_pri_procsinfo64 */
/* #undef _mem_positive_sign_lconv */
/* #undef _mem_pr_clname_prpsinfo */
/* #undef _mem_pr_gid_prpsinfo */
/* #undef _mem_pr_lttydev_prpsinfo */
/* #undef _mem_pr_npid_prpsinfo */
/* #undef _mem_pr_ntpid_prpsinfo */
/* #undef _mem_pr_pgid_prpsinfo */
/* #undef _mem_pr_pgrp_prpsinfo */
/* #undef _mem_pr_psargs_prpsinfo */
/* #undef _mem_pr_refcount_prpsinfo */
/* #undef _mem_pr_rssize_prpsinfo */
/* #undef _mem_pr_sid_prpsinfo */
/* #undef _mem_pr_sonproc_prpsinfo */
/* #undef _mem_pr_tgrp_prpsinfo */
/* #undef _mem_right_parenthesis_lconv */
#define _mem_sbrk 1
/* #undef _mem_st_blksize_stat */
/* #undef _mem_st_blocks_stat */
/* #undef _mem_st_rdev_stat */
/* #undef _mem_thousands_sep_lconv */
#define _mem_ut_host_utmp 1
#define _mem_ut_host_utmpx 1
#define _mem_ut_tv_utmp 1
#define _mem_ut_tv_utmpx 1
#define _mem_ut_type_utmp 1
/* #undef _mem_ut_type_utmpx */
#define _mem_ut_user_utmp 1
#define _mem_ut_user_utmpx 1
/* #undef _mem_win32 */

/* #undef _sys_file */
/* #undef _sys_filio */
/* #undef _sys_fss */
/* #undef _sys_ioctl */
/* #undef _sys_jioctl */
#define _sys_ldr 0
/* #undef _sys_param */
/* #undef _sys_proc */
/* #undef _sys_pstat */
/* #undef _sys_ptem */
/* #undef _sys_select */
#define _sys_socket 1
/* #undef _sys_sockio */
/* #undef _sys_stream */
/* #undef _sys_uio */
/* #undef _sys_un */
/* #undef _sys_universe */
/* #undef _sys_utsname */

/* #undef _typ___va_list */
/* #undef _typ_ino64_t */
/* #undef _typ_long_double */
/* #undef _typ_mbstate_t */
/* #undef _typ_off64_t */
/* #undef _typ_struct_stat64 */
