# Static Checkers options
CHKREPORT	     = $(REPORT_DIR)/cppcheck-report.txt
CHKFLAGS	     = --enable=all --error-exitcode=1 --std=c11 --suppress=missingIncludeSystem:nofile -D__GNUC__
CHKMISRAREPORT = $(REPORT_DIR)/misra-report.txt
CHKMISRAFLAGS  = -q -j4 --addon=misra --std=c11