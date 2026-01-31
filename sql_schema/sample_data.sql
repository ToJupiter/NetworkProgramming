-- sample_data.sql

-- Insert 30 sample users
INSERT INTO users (email, display_name, hashed_password) VALUES
('alice.johnson@email.com', 'AliceJ', '$2b$12$Xq.uAdsQ2EjdaBeMZuZDjumTw8eeELY0fm4seJfdhA91mX.mV.NeG'),
('bob.smith@email.com', 'BobSmith', '$2b$12$qsHzDYes4qfjUxrGzW.CLetqcc5amtyG1i0uQ9C3cxtHYbTYSi28K'),
('carol.davis@email.com', 'CarolD', '$2b$12$W1aiAqNLcd2qDstDNWFJFumVmolcfzYVvTN0ax/jmb60rB8p5B2nW'),
('david.wilson@email.com', 'DaveW', '$2b$12$y6joNuxRFGpCGiN4hLH2ouqtShCbV8nY3wXyjPJv059NH/zDpCcJK'),
('emma.brown@email.com', 'EmmaB', '$2b$12$PtTuXj8EMVe5fM2igOXaTOhi74E/aBE9Bb7AkQaf7D6Uv1YCamT8W'),
('frank.miller@email.com', 'FrankM', '$2b$12$eVgdjivxR6NnYNz4zNsbUOfXi/gMogm7GbRhVPt.lzhx8CSHRsBLm'),
('grace.taylor@email.com', 'GraceT', '$2b$12$GOPbMIbPb0MWEDgrsadxu.Kd71CDRii24JIyEbyf7EfhA.toYYaUS'),
('henry.anderson@email.com', 'HenryA', '$2b$12$HTEmkRWPEBZQK93FZvkCgeB87I5Lfo/5z95NEwdPvzU.ZIpdoaoSi'),
('ivy.martin@email.com', 'IvyM', '$2b$12$GqE0pWeh.n6.XhPgbBhxc.B07BGYk/PNwE/O.jR24FIKhcuEdaVfO'),
('jack.thomas@email.com', 'JackT', '$2b$12$LO/kqQNC6fR2yEG0WtjF7.HuvXEANn3Vbkicnc5x5aw59.BjGnuzO'),
('karen.white@email.com', 'KarenW', '$2b$12$V0T8..lNKWkrRi/TgHYPcOZxTOUV3wYv1gtfKZcD.YQ9yoHm/9vMq'),
('leo.moore@email.com', 'LeoM', '$2b$12$WmVyjo4MA33gQXPeG2DlWO4/CO5Eb4laQLaUwHFY5tzVqBh6vP8rC'),
('mia.jackson@email.com', 'MiaJ', '$2b$12$jQN8rRS7ikJ.H4lRhCaeceFpfyGWuW0sgtkt/G8IT35bmZmV9aNaa'),
('nathan.thompson@email.com', 'NathanT', '$2b$12$sd7UCDS1ZVpmnu0FOz48j.vrRORkrjcQNeej.ULiCMPPXj8wXKnvS'),
('olivia.garcia@email.com', 'OliviaG', '$2b$12$yQLNq01ExGuNFIrHvGfV2eFLgBWSS4pCWGVY3p7ROymV72rqNiZm6'),
('paul.martinez@email.com', 'PaulM', '$2b$12$KRr/VgIr3ZnGDMcuTgheqOq0vqnBxFA0cLyWJ8Ep/e6MEfjjDSRLy'),
('quincy.lee@email.com', 'QuincyL', '$2b$12$HOmPC8Im9tG.WxDssVd1r.ltHI7ZcsIJ3fmAMgGFB.OOIAa3L6Q7i'),
('rachel.harris@email.com', 'RachelH', '$2b$12$.axRzrzxboh4i5Fkwgm36.jeWGqsNTw4D2W1tBuz22ly.B4xyqjo6'),
('samuel.clark@email.com', 'SamC', '$2b$12$40wtmzIZ7z6TMgYf97Eo7e9mUlfuNCCwRXD2IkkzuMjg7.JsaVb1S'),
('tina.lewis@email.com', 'TinaL', '$2b$12$Vr8BSlDTKAf1Q3pOaNOy6uzwI/rgBhBtOKh4LcEwwUE0/eQ0mxwdW'),
('umar.walker@email.com', 'UmarW', '$2b$12$isSl5qmOCakw8kS52UNDKebJIv.cMDkWm/Zpve5.UmY7p71IXXlDa'),
('victoria.hall@email.com', 'VickyH', '$2b$12$KmgyakhX0cq.q.MfXR6SXugqHxnCThHvEQyJOkIRqhnU2ZhjvG0bS'),
('walter.young@email.com', 'WalterY', '$2b$12$5ik92r6t5W3XHrpzxNKfke3jafeDf2Bqb5SVFubgJanYV9XLvX2.m'),
('xena.king@email.com', 'XenaK', '$2b$12$FIGGISxNtKrOvSnJO0cR2uwJcHs5sF6yqfnpMbS0Gyv4izgnjiGN.'),
('yousef.scott@email.com', 'YousefS', '$2b$12$mdJCeioyCKKX2kQoXdFSoeAw8EqNnM2neCW1IjnPkE2Bn1OM7oIJm'),
('zoe.adams@email.com', 'ZoeA', '$2b$12$yf9yB9HtjOz1rntR9we5xu29eiikarsA3x2jz2tX7jbZHUQtQfqyW'),
('adam.green@email.com', 'AdamG', '$2b$12$LUglNUen9MDK2nyV3nREvuXfIcUa.luRwvdVKGAHI2Vuxo9NZewdy'),
('bella.hall@email.com', 'BellaH', '$2b$12$R0jQrFcjtFegbuzgYA.SQ.sUe96frnP41Em1ahok6kTm2c.04BTe2'),
('charlie.nelson@email.com', 'CharlieN', '$2b$12$JD12NUpuJyIhMJmfECqEvOk/HX.TdIGlmpqMEL6SBql78gZh.cOPW'),
('diana.mitchell@email.com', 'DianaM', '$2b$12$zsmfjqaht5xG2lK98lANp.hTT1tCjqrLDCRStQMnyP9jzF10lBDFC');


-- Insert sample questions (20 questions)
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

-- Insert game sessions (8 sessions)
INSERT INTO game_sessions (game_mode, created_at, ended_at) VALUES
('Elimination', '2024-01-15 10:00:00', '2024-01-15 10:30:00'),
('Scoring', '2024-01-15 14:00:00', '2024-01-15 14:45:00'),
('Elimination', '2024-01-16 09:30:00', '2024-01-16 10:15:00'),
('Scoring', '2024-01-16 16:00:00', '2024-01-16 16:50:00'),
('Elimination', '2024-01-17 11:00:00', '2024-01-17 11:40:00'),
('Scoring', '2024-01-17 15:30:00', '2024-01-17 16:20:00'),
('Elimination', '2024-01-18 13:00:00', '2024-01-18 13:35:00'),
('Scoring', '2024-01-18 18:00:00', '2024-01-18 18:55:00');

-- Insert session participants (varying numbers per session)
INSERT INTO session_participants (user_id, session_id, score, rank) VALUES
-- Session 1: 4 participants
(1, 1, 850, 1), (2, 1, 720, 2), (3, 1, 680, 3), (4, 1, 550, 4),
-- Session 2: 6 participants
(5, 2, 920, 1), (6, 2, 880, 2), (7, 2, 790, 3), (8, 2, 750, 4), (9, 2, 680, 5), (10, 2, 620, 6),
-- Session 3: 5 participants
(11, 3, 780, 1), (12, 3, 740, 2), (13, 3, 710, 3), (14, 3, 650, 4), (15, 3, 590, 5),
-- Session 4: 8 participants
(16, 4, 950, 1), (17, 4, 890, 2), (18, 4, 860, 3), (19, 4, 820, 4), (20, 4, 780, 5), (21, 4, 730, 6), (22, 4, 690, 7), (23, 4, 640, 8),
-- Session 5: 3 participants
(24, 5, 810, 1), (25, 5, 760, 2), (26, 5, 710, 3),
-- Session 6: 7 participants
(27, 6, 880, 1), (28, 6, 840, 2), (29, 6, 800, 3), (30, 6, 770, 4), (1, 6, 720, 5), (2, 6, 680, 6), (3, 6, 630, 7),
-- Session 7: 5 participants
(4, 7, 790, 1), (5, 7, 750, 2), (6, 7, 710, 3), (7, 7, 670, 4), (8, 7, 620, 5),
-- Session 8: 6 participants
(9, 8, 910, 1), (10, 8, 870, 2), (11, 8, 830, 3), (12, 8, 790, 4), (13, 8, 740, 5), (14, 8, 690, 6);

-- Insert game logs (sample answers for each participant in each session)
INSERT INTO game_log (session_id, user_id, question_id, selected_option, is_correct, response_time_ms) VALUES
-- Sample data for session 1 participants
(1, 1, 1, 3, 1, 2450), (1, 1, 2, 2, 1, 3200), (1, 1, 3, 2, 1, 1500),
(1, 2, 1, 3, 1, 3100), (1, 2, 2, 1, 0, 2800), (1, 2, 3, 2, 1, 1900),
(1, 3, 1, 4, 0, 4200), (1, 3, 2, 2, 1, 3500), (1, 3, 3, 1, 0, 2100),
(1, 4, 1, 3, 1, 3800), (1, 4, 2, 3, 0, 2900), (1, 4, 3, 3, 0, 3200),
-- Session 2 participants (sample)
(2, 5, 4, 2, 1, 2700), (2, 5, 5, 4, 1, 3300), (2, 5, 6, 2, 1, 4100),
(2, 6, 4, 2, 1, 2900), (2, 6, 5, 3, 0, 3600), (2, 6, 6, 2, 1, 3800),
-- Add more game_log entries as needed for other sessions...
(3, 11, 7, 3, 1, 2200), (3, 11, 8, 3, 1, 3100), (3, 11, 9, 3, 1, 1800),
(4, 16, 10, 3, 1, 3400), (4, 16, 11, 1, 1, 5200), (4, 16, 12, 3, 1, 2800),
(5, 24, 13, 3, 1, 1900), (5, 24, 14, 3, 1, 2300), (5, 24, 15, 2, 1, 2700),
(6, 27, 16, 2, 1, 2100), (6, 27, 17, 3, 1, 3800), (6, 27, 18, 3, 1, 1600),
(7, 4, 19, 3, 1, 2900), (7, 4, 20, 3, 1, 2400), (7, 4, 1, 3, 1, 3100),
(8, 9, 2, 2, 1, 2600), (8, 9, 3, 2, 1, 1400), (8, 9, 4, 2, 1, 3300);