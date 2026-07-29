# fmt: off

import pytest
from conftest import ShellTest

# a query that runs for a very long time without the timeout
SLOW_QUERY = "SELECT COUNT(*) FROM range(100000000) t1, range(10000) t2;"


def test_timeout_interrupts_query(shell):
    test = (
        ShellTest(shell)
        .statement(".timeout 100")
        .statement(SLOW_QUERY)
    )
    result = test.run()
    result.check_stderr("run time exceeded .timeout of 100 ms")


def test_timeout_streaming_result(shell):
    test = (
        ShellTest(shell)
        .statement(".mode csv")
        .statement(".timeout 100")
        .statement("SELECT * FROM range(100000000) t1, range(10000) t2;")
    )
    result = test.run()
    result.check_stderr("run time exceeded .timeout of 100 ms")


def test_timeout_command_line_argument(shell):
    test = (
        ShellTest(shell)
        .add_argument("-timeout", "100")
        .statement(SLOW_QUERY)
    )
    result = test.run()
    result.check_stderr("run time exceeded .timeout of 100 ms")


def test_timeout_query_completes_in_time(shell):
    test = (
        ShellTest(shell)
        .statement(".timeout 100000")
        .statement("SELECT 42 AS x;")
        .statement("SELECT 84 AS y;")
    )
    result = test.run()
    result.check_stdout("42")
    result.check_stdout("84")


def test_timeout_disable(shell):
    test = (
        ShellTest(shell)
        .statement(".timeout 100")
        .statement(".timeout 0")
        .statement("SELECT COUNT(*) FROM range(10000000) t(i);")
    )
    result = test.run()
    result.check_stdout("10000000")


def test_timeout_invalid_argument(shell):
    test = (
        ShellTest(shell)
        .statement(".timeout xyz")
    )
    result = test.run()
    result.check_stderr("expected a number of milliseconds")
