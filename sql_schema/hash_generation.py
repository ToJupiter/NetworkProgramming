import bcrypt

# List of users from the sample data (email, display_name)
users = [
    ('alice.johnson@email.com', 'AliceJ'),
    ('bob.smith@email.com', 'BobSmith'),
    ('carol.davis@email.com', 'CarolD'),
    ('david.wilson@email.com', 'DaveW'),
    ('emma.brown@email.com', 'EmmaB'),
    ('frank.miller@email.com', 'FrankM'),
    ('grace.taylor@email.com', 'GraceT'),
    ('henry.anderson@email.com', 'HenryA'),
    ('ivy.martin@email.com', 'IvyM'),
    ('jack.thomas@email.com', 'JackT'),
    ('karen.white@email.com', 'KarenW'),
    ('leo.moore@email.com', 'LeoM'),
    ('mia.jackson@email.com', 'MiaJ'),
    ('nathan.thompson@email.com', 'NathanT'),
    ('olivia.garcia@email.com', 'OliviaG'),
    ('paul.martinez@email.com', 'PaulM'),
    ('quincy.lee@email.com', 'QuincyL'),
    ('rachel.harris@email.com', 'RachelH'),
    ('samuel.clark@email.com', 'SamC'),
    ('tina.lewis@email.com', 'TinaL'),
    ('umar.walker@email.com', 'UmarW'),
    ('victoria.hall@email.com', 'VickyH'),
    ('walter.young@email.com', 'WalterY'),
    ('xena.king@email.com', 'XenaK'),
    ('yousef.scott@email.com', 'YousefS'),
    ('zoe.adams@email.com', 'ZoeA'),
    ('adam.green@email.com', 'AdamG'),
    ('bella.hall@email.com', 'BellaH'),
    ('charlie.nelson@email.com', 'CharlieN'),
    ('diana.mitchell@email.com', 'DianaM'),
]

# Default password to hash (change as needed)
default_password = b"password"

# Generate the INSERT statement with real bcrypt hashes
print("INSERT INTO users (email, display_name, hashed_password) VALUES")
for email, display_name in users:
    hashed_password = bcrypt.hashpw(default_password, bcrypt.gensalt()).decode('utf-8')
    print(f"('{email}', '{display_name}', '{hashed_password}'),")

# Note: Remove the trailing comma from the last line when using in SQL.