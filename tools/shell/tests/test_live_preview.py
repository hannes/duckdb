# fmt: off

import pytest
from conftest import ShellTest


def test_live_requires_a_terminal(shell):
    # the preview draws below the prompt, so it only makes sense on a terminal
    test = (
        ShellTest(shell)
        .statement('.live on')
    )
    result = test.run()
    result.check_stderr("interactive terminal")


def test_live_missing_argument(shell):
    test = (
        ShellTest(shell)
        .statement('.live')
    )
    result = test.run()
    result.check_stderr("Usage")


def test_live_off_without_on(shell):
    # turning it off when it was never on is not an error - there is simply nothing to tear down
    test = (
        ShellTest(shell)
        .statement('.live off')
        .statement('SELECT 42;')
    )
    result = test.run()
    result.check_stdout("42")


def test_live_on_requires_a_terminal_but_off_does_not(shell):
    # -interactive forces interactive input, but stdout is still a pipe here
    test = (
        ShellTest(shell)
        .add_argument('-interactive')
        .statement('.live off')
        .statement('SELECT 42;')
    )
    result = test.run()
    result.check_stdout("42")
