#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;

// File is saved next to the program, so it works on any computer
const string FILE_NAME = "students.txt";
const int MAX_STUDENTS = 100;
const int MAX_LOGIN_ATTEMPTS = 3;

struct Student
{
    int id;
    string name;
    int age;
    string department;
};

Student students[MAX_STUDENTS];
int totalStudents = 0;

int findStudent(int id);

// ---------------------------------------------------------
// Input helpers (safe against letters, blank lines, etc.)
// ---------------------------------------------------------
string readLine()
{
    string line;

    if (!getline(cin, line))
    {
        // Input stream closed (Ctrl+Z / Ctrl+D): exit cleanly
        cout << "\nInput closed. Exiting program.\n";
        exit(0);
    }

    return line;
}

int readInt()
{
    while (true)
    {
        string line = readLine();
        stringstream ss(line);
        int value;
        char extra;

        // Must be a valid number with nothing extra after it
        if (ss >> value && !(ss >> extra))
        {
            return value;
        }

        cout << "Invalid input. Please enter a number: ";
    }
}

bool isBlank(const string &text)
{
    for (size_t i = 0; i < text.length(); i++)
    {
        if (text[i] != ' ' && text[i] != '\t')
        {
            return false;
        }
    }

    return true;
}

// Reads text that must not be empty and must not contain '|'
string readText(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        string text = readLine();

        if (isBlank(text))
        {
            cout << "This field cannot be empty.\n";
        }
        else if (text.find('|') != string::npos)
        {
            cout << "The character '|' is not allowed.\n";
        }
        else
        {
            return text;
        }
    }
}

int readAge(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        int age = readInt();

        if (age > 0 && age <= 120)
        {
            return age;
        }

        cout << "Please enter a valid age (1-120).\n";
    }
}

int readId(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        int id = readInt();

        if (id > 0)
        {
            return id;
        }

        cout << "Student ID must be a positive number.\n";
    }
}

void printStudent(const Student &s)
{
    cout << "Student ID: " << s.id << "\n";
    cout << "Name: " << s.name << "\n";
    cout << "Age: " << s.age << "\n";
    cout << "Department: " << s.department << "\n";
}

// ---------------------------------------------------------
// File handling
// ---------------------------------------------------------
bool saveStudents()
{
    ofstream file(FILE_NAME.c_str());

    if (!file.is_open())
    {
        cout << "\nERROR: Could not open " << FILE_NAME << " for saving.\n";
        cout << "Check that the folder is writable.\n";
        return false;
    }

    for (int i = 0; i < totalStudents; i++)
    {
        file << students[i].id << "|"
             << students[i].name << "|"
             << students[i].age << "|"
             << students[i].department << "\n";
    }

    file.close();

    if (file.fail())
    {
        cout << "\nERROR: Could not save student records.\n";
        return false;
    }

    cout << "\nStudent records saved successfully!\n";
    return true;
}

void loadStudents()
{
    ifstream file(FILE_NAME.c_str());

    if (!file.is_open())
    {
        cout << "\nNo saved records found yet.\n";
        cout << "A file will be created when you add a student.\n";
        return;
    }

    string line;

    while (getline(file, line))
    {
        if (totalStudents >= MAX_STUDENTS)
        {
            cout << "\nMaximum of " << MAX_STUDENTS << " students reached.\n";
            break;
        }

        stringstream ss(line);
        string idText, name, ageText, department;

        if (getline(ss, idText, '|') &&
            getline(ss, name, '|') &&
            getline(ss, ageText, '|') &&
            getline(ss, department))
        {
            stringstream idStream(idText);
            stringstream ageStream(ageText);

            int id, age;

            if (idStream >> id && ageStream >> age)
            {
                if (findStudent(id) == -1)
                {
                    students[totalStudents].id = id;
                    students[totalStudents].name = name;
                    students[totalStudents].age = age;
                    students[totalStudents].department = department;

                    totalStudents++;
                }
            }
        }
    }

    file.close();

    cout << "\nLoaded student records: " << totalStudents << "\n";
}

// ---------------------------------------------------------
// Core functions
// ---------------------------------------------------------
int findStudent(int id)
{
    for (int i = 0; i < totalStudents; i++)
    {
        if (students[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

bool adminLogin()
{
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++)
    {
        cout << "\n===== ADMIN LOGIN =====\n";
        cout << "Enter Username: ";
        string username = readLine();

        cout << "Enter Password: ";
        string password = readLine();

        if (username == "admin" && password == "1234")
        {
            cout << "\nLogin Successful!\n";
            return true;
        }

        cout << "\nIncorrect Username or Password!";
        cout << " (Attempt " << attempt << " of "
             << MAX_LOGIN_ATTEMPTS << ")\n";
    }

    cout << "\nToo many failed attempts. Returning to main menu.\n";
    return false;
}

void addStudent()
{
    if (totalStudents >= MAX_STUDENTS)
    {
        cout << "\nStudent limit reached!\n";
        return;
    }

    cout << "\n===== ADD STUDENT =====\n";

    int id = readId("Enter Student ID: ");

    if (findStudent(id) != -1)
    {
        cout << "\nThis Student ID already exists!\n";
        return;
    }

    string name = readText("Enter Student Name: ");
    int age = readAge("Enter Age: ");
    string department = readText("Enter Department: ");

    students[totalStudents].id = id;
    students[totalStudents].name = name;
    students[totalStudents].age = age;
    students[totalStudents].department = department;

    totalStudents++;

    if (!saveStudents())
    {
        totalStudents--;
        cout << "Student was not added because saving failed.\n";
        return;
    }

    cout << "Student added successfully!\n";
}

void viewStudents()
{
    if (totalStudents == 0)
    {
        cout << "\nNo student records found.\n";
        return;
    }

    for (int i = 0; i < totalStudents; i++)
    {
        cout << "\n--------------------------\n";
        printStudent(students[i]);
    }

    cout << "\nTotal Students: " << totalStudents << "\n";
}

void searchStudent()
{
    int id = readId("\nEnter Student ID to search: ");

    int index = findStudent(id);

    if (index == -1)
    {
        cout << "\nStudent not found!\n";
    }
    else
    {
        cout << "\n===== STUDENT FOUND =====\n";
        printStudent(students[index]);
    }
}

void updateStudent()
{
    int id = readId("\nEnter Student ID to update: ");

    int index = findStudent(id);

    if (index == -1)
    {
        cout << "\nStudent not found!\n";
        return;
    }

    Student oldStudent = students[index];

    // Read into temporary variables first, validate, then apply
    string newName = readText("Enter New Name: ");
    int newAge = readAge("Enter New Age: ");
    string newDepartment = readText("Enter New Department: ");

    students[index].name = newName;
    students[index].age = newAge;
    students[index].department = newDepartment;

    if (!saveStudents())
    {
        students[index] = oldStudent;
        cout << "Changes cancelled because saving failed.\n";
        return;
    }

    cout << "\nStudent updated successfully!\n";
}

void deleteStudent()
{
    int id = readId("\nEnter Student ID to delete: ");

    int index = findStudent(id);

    if (index == -1)
    {
        cout << "\nStudent not found!\n";
        return;
    }

    cout << "Are you sure you want to delete "
         << students[index].name << "? (y/n): ";
    string confirm = readLine();

    if (confirm != "y" && confirm != "Y")
    {
        cout << "\nDeletion cancelled.\n";
        return;
    }

    Student deletedStudent = students[index];

    for (int i = index; i < totalStudents - 1; i++)
    {
        students[i] = students[i + 1];
    }

    totalStudents--;

    if (!saveStudents())
    {
        // Roll back: shift records back and restore the deleted one
        for (int i = totalStudents; i > index; i--)
        {
            students[i] = students[i - 1];
        }

        students[index] = deletedStudent;
        totalStudents++;

        cout << "Deletion cancelled because saving failed.\n";
        return;
    }

    cout << "\nStudent deleted successfully!\n";
}

// ---------------------------------------------------------
// Menus
// ---------------------------------------------------------
void adminPanel()
{
    if (!adminLogin())
    {
        return;
    }

    int choice;

    do
    {
        cout << "\n========== ADMIN PANEL ==========\n";
        cout << "1. Add Student\n";
        cout << "2. View All Students\n";
        cout << "3. Search Student\n";
        cout << "4. Update Student\n";
        cout << "5. Delete Student\n";
        cout << "6. Logout\n";
        cout << "Enter your choice: ";

        choice = readInt();

        if (choice == 1)
            addStudent();
        else if (choice == 2)
            viewStudents();
        else if (choice == 3)
            searchStudent();
        else if (choice == 4)
            updateStudent();
        else if (choice == 5)
            deleteStudent();
        else if (choice == 6)
            cout << "\nAdmin logged out successfully!\n";
        else
            cout << "\nInvalid choice!\n";

    } while (choice != 6);
}

void studentPanel()
{
    cout << "\n===== STUDENT LOGIN =====\n";
    int id = readId("Enter Your Student ID: ");

    int index = findStudent(id);

    if (index == -1)
    {
        cout << "\nStudent ID not found!\n";
        return;
    }

    cout << "\nStudent Login Successful!\n";

    int choice;

    do
    {
        cout << "\n========== STUDENT PANEL ==========\n";
        cout << "1. View My Profile\n";
        cout << "2. Logout\n";
        cout << "Enter your choice: ";

        choice = readInt();

        if (choice == 1)
        {
            cout << "\n===== MY PROFILE =====\n";
            printStudent(students[index]);
        }
        else if (choice == 2)
        {
            cout << "\nStudent logged out successfully!\n";
        }
        else
        {
            cout << "\nInvalid choice! Try again.\n";
        }

    } while (choice != 2);
}

int main()
{
    loadStudents();

    int choice;

    do
    {
        cout << "\n================================\n";
        cout << "    STUDENT MANAGEMENT SYSTEM\n";
        cout << "================================\n";
        cout << "1. Admin Panel\n";
        cout << "2. Student Panel\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        choice = readInt();

        if (choice == 1)
            adminPanel();
        else if (choice == 2)
            studentPanel();
        else if (choice == 3)
            cout << "\nThank you for using our system!\n";
        else
            cout << "\nInvalid choice!\n";

    } while (choice != 3);

    return 0;
}
