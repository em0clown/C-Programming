import pytest
from utils import format_username

def test_format_username_standard():
    assert format_username("george", "kolesov") == "George Kolesov"

def test_format_username_with_spaces():
    assert format_username("  ivan  ", "ivanov") == "Ivan Ivanov"

def test_format_username_empty_field():
    assert format_username("", "Kolesov") == "Unknown User"

def test_format_username_none():
    assert format_username(None, None) == "Unknown User"