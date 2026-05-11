def format_username(first_name, last_name):
    if not first_name or not last_name:
        return "Unknown User"
    return f"{first_name.strip()} {last_name.strip()}".title()