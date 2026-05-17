#include <check.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int msgSend(char* msg);
int msgRecv(char* buf);

START_TEST(test_regular_usage) {
    char test_msg[] = "[Klient_1] Test message for FIFO";
    
    msgSend(test_msg);

    char buffer[128];
    int recv_len = msgRecv(buffer);
    
    ck_assert_int_eq(recv_len, strlen(test_msg));
    ck_assert_str_eq(buffer, test_msg);
}
END_TEST

START_TEST(test_message_truncation) {
    char long_msg[200];
    memset(long_msg, 'A', 127);
    memset(long_msg + 127, 'B', 72); 
    long_msg[199] = '\0';

    msgSend(long_msg);

    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    int recv_len = msgRecv(buffer);
    ck_assert_int_eq(recv_len, 127);
    ck_assert_uint_eq(strlen(buffer), 127);
}
END_TEST

Suite *queue_chat_suite(void) {
    Suite *s = suite_create("Queue_Chat_Tests");
    TCase *tc_core = tcase_create("Core_Logic");
    tcase_add_test(tc_core, test_regular_usage);
    tcase_add_test(tc_core, test_message_truncation);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s = queue_chat_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK); 
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}