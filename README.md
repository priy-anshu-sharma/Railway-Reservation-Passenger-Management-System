# Railway-Reservation-Passenger-Management-System
A C-based Railway Reservation &amp; Passenger Management System that handles train and passenger records using file handling. It allows users to book, cancel, search, and update tickets, manage seat availability, add new trains, and view all reservations with secure data storage and automatic PNR generation.
Project Title:

Railway Reservation & Passenger Management System

Project Description:

The Railway Reservation & Passenger Management System is a C language–based console application designed to manage train and passenger records efficiently. It aims to simplify the process of booking, cancelling, and maintaining railway reservations while ensuring data consistency and ease of use.

This project uses file handling (reservations.dat, trains.dat) to permanently store passenger details and train information, enabling data persistence even after the program closes. It provides a menu-driven interface for performing all major operations — such as viewing available trains, booking tickets, cancelling reservations, searching passengers by name, and updating records.

The system maintains a list of trains (each having unique train numbers, names, and seat capacities) and allows passengers to reserve seats by automatically generating unique PNR numbers. The program also updates seat availability dynamically after each booking or cancellation.

Key operations are divided into modules for clarity and reliability:

Booking & Cancellation: Handles ticket reservation, assigns seat numbers, and manages seat availability.

Data Viewing: Displays all trains, specific ticket details (by PNR), or all reservations.

Search & Update: Enables modification of passenger details and name-based searches.

Maintenance: Cleans up cancelled records and supports adding new trains or resetting the entire database.

The system is designed with robust error checking, input validation, and safe file operations to prevent data loss. It also supports scalability by allowing multiple trains (up to 20) and up to 50 seats per train.

Key Features:

Interactive, menu-based user interface.

Secure data handling with .dat binary files.

Automatic PNR generation and seat assignment.

Real-time seat availability updates.

Passenger search and record modification options.

Data cleanup and train addition features.

Complete database reset functionality for maintenance.

Technologies Used:

Language: C

Concepts: File Handling, Structures, Loops, Conditional Logic, Data Persistence

Files: reservations.dat and trains.dat

Outcome:

This project demonstrates practical implementation of file management and modular programming in C, while addressing a real-world problem — digital railway booking management. It strengthens the understanding of data storage, record manipulation, and structured programming techniques applicable to many real-life management systems.
