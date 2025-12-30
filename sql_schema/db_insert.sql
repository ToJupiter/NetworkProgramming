ALTER TABLE users ADD COLUMN ranked_points INTEGER DEFAULT 1000;

-- schema.sql

-- Table to store user account information
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    email TEXT UNIQUE NOT NULL,
    display_name TEXT NOT NULL,
    hashed_password TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    ranked_points INTEGER DEFAULT 1000
);

-- Table to store all quiz questions
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content TEXT NOT NULL,
    option1 TEXT NOT NULL,
    option2 TEXT NOT NULL,
    option3 TEXT NOT NULL,
    option4 TEXT NOT NULL,
    correct_option INTEGER NOT NULL, -- 1, 2, 3, or 4
    difficulty INTEGER DEFAULT 1 -- e.g., 1 for easy, 2 for medium, etc.
);

-- Table to log each game session/match that occurs
CREATE TABLE IF NOT EXISTS game_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    game_mode TEXT NOT NULL, -- e.g., 'Elimination', 'Scoring'
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    ended_at DATETIME
);

-- Table to link users to game sessions and store their results
-- This table is essential for tracking history and statistics
CREATE TABLE IF NOT EXISTS session_participants (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_id INTEGER NOT NULL,
    score INTEGER DEFAULT 0,
    rank INTEGER, -- Final rank in the game (e.g., 1 for winner)
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(session_id) REFERENCES game_sessions(id)
);

-- Table to log every single answer from every player (for replay functionality)
CREATE TABLE IF NOT EXISTS game_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    question_id INTEGER NOT NULL,
    selected_option INTEGER NOT NULL,
    is_correct BOOLEAN NOT NULL,
    response_time_ms INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(session_id) REFERENCES game_sessions(id),
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(question_id) REFERENCES questions(id)
);

INSERT INTO questions (content, option1, option2, option3, option4, correct_option, difficulty) VALUES
('What is the capital of France?', 'London', 'Berlin', 'Paris', 'Madrid', 3, 1),
('Which planet is known as the Red Planet?', 'Venus', 'Mars', 'Jupiter', 'Saturn', 2, 1),
('What is 2 + 2?', '3', '4', '5', '6', 2, 1),
('Who wrote "Romeo and Juliet"?', 'Charles Dickens', 'William Shakespeare', 'Jane Austen', 'Mark Twain', 2, 1),
('What is the largest ocean on Earth?', 'Atlantic', 'Indian', 'Arctic', 'Pacific', 4, 1),
('In which year did World War II end?', '1944', '1945', '1946', '1947', 2, 2),
('What is the chemical symbol for gold?', 'Go', 'Gd', 'Au', 'Ag', 3, 2),
('Which programming language was created by Bjarne Stroustrup?', 'Java', 'Python', 'C++', 'C#', 3, 2),
('What is the square root of 64?', '6', '7', '8', '9', 3, 1),
('Which country has the largest population?', 'India', 'United States', 'China', 'Russia', 3, 2),
('What is the speed of light in vacuum?', '299,792 km/s', '300,000 km/s', '299,792 m/s', '300,000 m/s', 1, 3),
('Who painted the Mona Lisa?', 'Vincent van Gogh', 'Pablo Picasso', 'Leonardo da Vinci', 'Michelangelo', 3, 1),
('What is the hardest natural substance on Earth?', 'Gold', 'Iron', 'Diamond', 'Platinum', 3, 1),
('Which element has the atomic number 1?', 'Helium', 'Oxygen', 'Hydrogen', 'Carbon', 3, 1),
('What is the largest mammal?', 'Elephant', 'Blue Whale', 'Giraffe', 'Polar Bear', 2, 1),
('In computing, what does CPU stand for?', 'Computer Processing Unit', 'Central Processing Unit', 'Central Program Unit', 'Computer Program Unit', 2, 1),
('Which gas is most abundant in Earth''s atmosphere?', 'Oxygen', 'Carbon Dioxide', 'Nitrogen', 'Hydrogen', 3, 2),
('What is the smallest prime number?', '0', '1', '2', '3', 3, 1),
('Which country is known as the Land of the Rising Sun?', 'China', 'Thailand', 'Japan', 'South Korea', 3, 1),
('What is the main ingredient in guacamole?', 'Tomato', 'Onion', 'Avocado', 'Pepper', 3, 1);

INSERT INTO questions (content, option1, option2, option3, option4, correct_option, difficulty) VALUES
('Which language is spoken in Brazil?', 'Spanish', 'Portuguese', 'French', 'English', 2, 1),
('What is the color of the sun on a clear day?', 'White', 'Yellow', 'Orange', 'Red', 1, 1),
('How many sides does a triangle have?', '2', '3', '4', '5', 2, 1),
('What do bees produce?', 'Silk', 'Honey', 'Wax', 'Vinegar', 2, 1),
('Which animal is known as the "king of the jungle"?', 'Elephant', 'Lion', 'Tiger', 'Bear', 2, 1),
('What is the freezing point of water in Celsius?', '-10°C', '0°C', '10°C', '100°C', 2, 1),
('Which continent is the largest?', 'Africa', 'Asia', 'North America', 'Europe', 2, 2),
('How many players are on a soccer team?', '9', '10', '11', '12', 3, 1),
('What is the capital of Japan?', 'Seoul', 'Beijing', 'Tokyo', 'Bangkok', 3, 1),
('Which planet is closest to the sun?', 'Venus', 'Earth', 'Mars', 'Mercury', 4, 2),

('In which year did the Titanic sink?', '1905', '1912', '1918', '1923', 2, 2),
('What is the chemical symbol for silver?', 'Si', 'S', 'Ag', 'Au', 3, 2),
('Who developed the theory of relativity?', 'Isaac Newton', 'Niels Bohr', 'Albert Einstein', 'Stephen Hawking', 3, 2),
('What is the largest desert in the world?', 'Sahara', 'Arabian', 'Gobi', 'Antarctic', 4, 2),
('How many bones are in the adult human body?', '196', '206', '216', '226', 2, 2),
('Which blood type is the universal donor?', 'A', 'B', 'AB', 'O', 4, 3),
('What is the smallest country in the world?', 'Monaco', 'San Marino', 'Vatican City', 'Liechtenstein', 3, 2),
('Which organ produces insulin?', 'Liver', 'Kidney', 'Pancreas', 'Stomach', 3, 2),
('What does HTML stand for?', 'Hyper Text Markup Language', 'High Tech Modern Language', 'Hyper Transfer Markup Language', 'Home Tool Markup Language', 1, 2),
('Which planet has the most moons?', 'Jupiter', 'Saturn', 'Uranus', 'Neptune', 2, 3),

('What is the value of π to two decimal places?', '3.14', '3.16', '3.41', '3.12', 1, 1),
('Who discovered penicillin?', 'Marie Curie', 'Alexander Fleming', 'Louis Pasteur', 'Robert Koch', 2, 2),
('What is the atomic number of carbon?', '4', '6', '8', '12', 2, 3),
('Which programming language is known for its use in web development alongside HTML and CSS?', 'Python', 'Java', 'JavaScript', 'C++', 3, 2),
('What is the rarest blood type in humans?', 'AB-', 'B-', 'O-', 'A-', 1, 3),
('In physics, what does "c" represent?', 'Speed of sound', 'Speed of light', 'Specific heat', 'Charge of electron', 2, 3),
('Which mathematician formulated the laws of motion?', 'Pythagoras', 'Isaac Newton', 'Galileo Galilei', 'Archimedes', 2, 2),
('What is the most abundant gas in the Earth''s atmosphere?', 'Oxygen', 'Carbon Dioxide', 'Nitrogen', 'Argon', 3, 3),
('Which element is liquid at room temperature?', 'Mercury', 'Bromine', 'Both Mercury and Bromine', 'Gallium', 3, 3),
('What is the main component of the sun?', 'Helium', 'Oxygen', 'Hydrogen', 'Carbon', 3, 3),

('In which year was the first iPhone released?', '2005', '2007', '2009', '2010', 2, 2),
('What does VPN stand for?', 'Virtual Private Network', 'Verified Public Network', 'Virtual Public Network', 'Verified Private Network', 1, 3),
('Which algorithm is the basis for Bitcoin?', 'Proof of Stake', 'Proof of Work', 'Byzantine Fault Tolerance', 'Directed Acyclic Graph', 2, 4),
('What is the time complexity of binary search?', 'O(n)', 'O(n log n)', 'O(log n)', 'O(1)', 3, 4),
('Which data structure uses LIFO (Last In First Out) principle?', 'Queue', 'Stack', 'Array', 'Linked List', 2, 3),
('In quantum computing, what is a qubit?', 'Quantum transistor', 'Quantum bit', 'Quantum gate', 'Quantum register', 2, 4),
('Which protocol is used for secure web browsing?', 'HTTP', 'FTP', 'HTTPS', 'SMTP', 3, 3),
('What is the capital of Bhutan?', 'Kathmandu', 'Thimphu', 'Dhaka', 'Male', 2, 3),
('Which philosopher wrote "Thus Spoke Zarathustra"?', 'Immanuel Kant', 'Friedrich Nietzsche', 'Søren Kierkegaard', 'Jean-Paul Sartre', 2, 4),
('Who painted "The Starry Night"?', 'Claude Monet', 'Vincent van Gogh', 'Edvard Munch', 'Salvador Dalí', 2, 2),

('What is the hardest problem in computer science?', 'Traveling Salesman', 'P vs NP', 'Halting Problem', 'Sorting', 2, 4),
('Which theorem states that no consistent system of axioms can prove all truths about arithmetic?', 'Banach-Tarski Paradox', 'Gödel''s Incompleteness Theorems', 'Church-Turing Thesis', 'Fermat''s Last Theorem', 2, 4),
('What is the derivative of e^x?', '0', 'e^x', 'ln(x)', 'x*e^(x-1)', 2, 3),
('In Greek mythology, who was the king of the gods?', 'Poseidon', 'Hades', 'Zeus', 'Apollo', 3, 2),
('Which novel begins with "It was the best of times, it was the worst of times"?', 'Moby Dick', 'A Tale of Two Cities', 'Pride and Prejudice', 'War and Peace', 2, 3),
('What is the deepest point in the ocean?', 'Puerto Rico Trench', 'Java Trench', 'Mariana Trench', 'Tonga Trench', 3, 3),
('Which composer went deaf in his later years?', 'Mozart', 'Bach', 'Beethoven', 'Chopin', 3, 2),
('What is the approximate age of the universe?', '4.5 billion years', '13.8 billion years', '24.6 billion years', '1 trillion years', 2, 4),
('Which particle gives other particles mass via the Higgs mechanism?', 'Photon', 'Gluon', 'Higgs boson', 'Neutrino', 3, 4),
('What is the largest prime number less than 100?', '89', '91', '97', '99', 3, 3);