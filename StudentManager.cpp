#include <iostream>
#include <string>
#include <vector>
// #include <array>
#include <algorithm>
#include <fstream>
#include <sstream>

#define MIN_COURSE_CAPACITY 5
#define MAX_STUDENT_COURSES 20

using namespace std;

const string students_file = "students.txt";
const string courses_file = "courses.txt";
const string grades_file = "grades.txt";

class Person;
class Student;
class Course;
class Enrollment;
class FileHandler;

struct Grade
{
    Student *student;
    Course *course;
    float grade;
    bool operator==(const Grade &other) const;
};

vector<Student *> STUDENTS;
vector<Course *> COURSES;
vector<Grade> GRADES;

class Person
{
protected:
    string name;
    int age;
    long int phone_number;

public:
    Person(string name, int age, long int phone_number)
    {
        this->set_name(name);
        this->set_age(age);
        this->set_phone_number(phone_number);
    }

    Person() = default;

    string get_name()
    {
        return this->name;
    }

    void set_name(string name)
    {
        if (name.length() > 0)
            this->name = name;
    }

    int get_age()
    {
        return this->age;
    }

    void set_age(int age)
    {
        if (age > 0)
            this->age = age;
    }

    long int get_phone_number()
    {
        return this->phone_number;
    }

    void set_phone_number(long int phone_num)
    {
        if (phone_num >= 1000)
        {
            this->phone_number = phone_num;
        }
    }

    virtual void show_info()
    {
        cout << "Person name : " << this->name << endl;
        cout << "Person age : " << this->age << endl;
        cout << "Person phone number : " << this->phone_number << endl;
    }
};

class Student : public Person
{
private:
    int id;
    vector<Course *> courses;

    void remove_grades()
    {
        for (auto i : GRADES)
        {
            if (i.student->get_student_id() == this->id)
            {
                GRADES.erase(remove(GRADES.begin(), GRADES.end(), i), GRADES.end());
            }
        }
    }

public:
    int get_student_id()
    {
        return this->id;
    }

    static bool id_is_unique(int id)
    {
        for (auto i : STUDENTS)
        {
            if (i->get_student_id() == id)
                return false;
        }
        return true;
    }

    void set_student_id(int id)
    {
        if (!this->id_is_unique(id))
        {
            cout << "A student with id " << id << " already exists!" << endl;
            return;
        }
        this->id = id;
    }

    void add_course(Course *crs)
    {
        this->courses.push_back(crs);
    }

    bool can_attend_more_courses()
    {
        if (this->courses.size() < MAX_STUDENT_COURSES)
            return true;
        else
            return false;
    }

    void drop_course(Course *crs);

    vector<Course *> get_enrolled_courses()
    {
        return this->courses;
    }

    void show_info() override;

    float get_avg_grade();

    static Student *get_student_by_id(int id)
    {
        for (auto i : STUDENTS)
        {
            if (i->get_student_id() == id)
                return i;
        }
        throw runtime_error("No student with id \"" + to_string(id) + "\" was found!");
    }

    Student(string name, int id, int age, long int phone_number)
    {
        this->set_name(name);
        this->set_student_id(id);
        this->set_age(age);
        this->set_phone_number(phone_number);
        STUDENTS.push_back(this);
    }

    void remove_student();

    // Student() = default;
};

class Course
{
private:
    string title;
    int id;
    int capacity;
    string description;
    vector<Student *> students;
    bool is_mandatory;

    void remove_grades()
    {
        for (auto i : GRADES)
        {
            if (i.course->get_id() == this->id)
            {
                GRADES.erase(remove(GRADES.begin(), GRADES.end(), i), GRADES.end());
            }
        }
    }

public:
    int get_id()
    {
        return this->id;
    }

    static bool id_is_unique(int id)
    {
        for (auto i : COURSES)
        {
            if (i->get_id() == id)
                return false;
        }
        return true;
    }

    void set_id(int id)
    {
        if (!id_is_unique(id))
        {
            cout << "A course with the id " << id << " already exists!" << endl;
            return;
        }
        this->id = id;
    }

    string get_title()
    {
        return this->title;
    }

    void set_title(string title)
    {
        if (title.length() > 0)
            this->title = title;
    }

    int get_capacity()
    {
        return this->capacity;
    }

    void set_capacity(int capacity)
    {
        if (capacity >= MIN_COURSE_CAPACITY)
            this->capacity = capacity;
        else
            this->capacity = 20; // the deafult capacity, just in case
    }

    bool get_is_mandatory()
    {
        return this->is_mandatory;
    }

    string get_description()
    {
        return this->description;
    }

    void set_description(string desc)
    {
        if (desc.length() > 0)
        {
            this->description = desc;
        }
    }

    bool has_capacity()
    {
        if (this->students.size() < this->capacity)
            return true;
        else
            return false;
    }

    int student_count()
    {
        return this->students.size();
    }

    void add_student(Student *std)
    {
        this->students.push_back(std);
    }

    void drop_student(Student *std)
    {
        this->students.erase(remove(this->students.begin(), this->students.end(), std), this->students.end());
    }

    vector<Student *> get_attending_students()
    {
        return this->students;
    }

    void show_info();

    static Course *get_course_by_id(int id)
    {
        for (auto i : COURSES)
        {
            if (i->get_id() == id)
                return i;
        }
        throw runtime_error("No course with id \"" + to_string(id) + "\" was found!");
    }

    Course(int id, string title, int capacity, bool is_mandatory)
    {
        this->set_id(id);
        this->set_title(title);
        this->set_capacity(capacity);
        this->is_mandatory = is_mandatory; // you can't change whether a course is mandatory or not after creating it
        this->description = "";
        COURSES.push_back(this);
    }

    void remove_course();

    // Course() = default;
};

class Enrollment
{
private:
    static bool student_exists(int id)
    {
        for (auto i : STUDENTS)
        {
            if (i->get_student_id() == id)
                return true;
        }
        return false;
    }

    static bool course_exists(int id)
    {
        for (auto i : COURSES)
        {
            if (i->get_id() == id)
                return true;
        }
        return false;
    }

    static bool student_is_attending_course(int student_id, int course_id)
    {
        Student *std = Student::get_student_by_id(student_id);
        for (auto i : std->get_enrolled_courses())
        {
            if (i->get_id() == course_id)
                return true;
        }
        return false;
    }

    static void remove_grades(int student_id, int course_id)
    {
        for (auto i : GRADES)
        {
            if (i.course->get_id() == course_id && i.student->get_student_id() == student_id)
            {
                GRADES.erase(remove(GRADES.begin(), GRADES.end(), i), GRADES.end());
                return;
            }
        }
    }

public:
    static void enroll_to_course(int student_id, int course_id)
    {
        if (!student_exists(student_id))
        {
            cout << "No student with the id of " << student_id << " exists!" << endl;
            return;
        }

        if (!course_exists(course_id))
        {
            cout << "No course with the id of " << student_id << " exists!" << endl;
            return;
        }

        Student *std = Student::get_student_by_id(student_id);
        Course *crs = Course::get_course_by_id(course_id);

        if (!crs->has_capacity())
        {
            cout << "The course does not have any free spots!" << endl;
            return;
        }

        if (!std->can_attend_more_courses())
        {
            cout << "The student has reached the course limit! (" << MAX_STUDENT_COURSES << " courses)" << endl;
            return;
        }

        if (student_is_attending_course(student_id, course_id))
        {
            cout << "The student \"" << std->get_name() << "\" is already attending course \"" << crs->get_title() << "\" ." << endl;
            return;
        }

        std->add_course(crs);
        crs->add_student(std);

        cout << "Student \"" << std->get_name() << "\" enrolled to course \"" << crs->get_title() << "\" ." << endl;
    }

    static void drop_out_of_course(int student_id, int course_id)
    {
        if (!student_exists(student_id))
        {
            cout << "No student with the id of " << student_id << " exists!" << endl;
            return;
        }

        if (!course_exists(course_id))
        {
            cout << "No course with the id of " << student_id << " exists!" << endl;
            return;
        }

        // Student* std = Student::get_student_by_id(student_id);
        Course *crs = Course::get_course_by_id(course_id);

        vector<Student *> students = crs->get_attending_students();

        remove_grades(student_id, course_id);

        for (auto i : students)
        {
            if (i->get_student_id() == student_id)
            {
                crs->drop_student(i);
                i->drop_course(crs);
                cout << "Student \"" << i->get_name() << "\" dropped out of course \"" << crs->get_title() << "\" ." << endl;
                return;
            }
        }

        cout << "\nThe student is not attending this course!" << endl;
    }

    static vector<Grade> get_student_grades(int student_id)
    {
        vector<Grade> grades;

        if (!student_exists(student_id))
        {
            cout << "No student with the id of " << student_id << " exists!" << endl;
            return grades;
        }

        for (auto i : GRADES)
        {
            if (i.student->get_student_id() == student_id)
                grades.push_back(i);
        }

        return grades;
    }

    static vector<Grade> get_course_grades(int course_id)
    {
        vector<Grade> grades;

        if (!course_exists(course_id))
        {
            cout << "No course with the id of " << course_id << " exists!" << endl;
            return grades;
        }

        for (auto i : GRADES)
        {
            if (i.course->get_id() == course_id)
                grades.push_back(i);
        }

        return grades;
    }

    static void set_naw_grade(int student_id, int course_id, float grade)
    {
        if (!student_exists(student_id))
        {
            cout << "No student with the id of " << student_id << " exists!" << endl;
            return;
        }

        if (!course_exists(course_id))
        {
            cout << "No course with the id of " << course_id << " exists!" << endl;
            return;
        }

        if (!student_is_attending_course(student_id, course_id))
        {
            cout << "This student is not attending this course!" << endl;
            return;
        }

        for (auto i : GRADES)
        {
            if (i.student->get_student_id() == student_id && i.course->get_id() == course_id)
            {
                i.grade = grade;
                cout << "The grade " << grade << " was successfully set!" << endl;
                return;
            }
        }

        Grade new_grade;
        new_grade.student = Student::get_student_by_id(student_id);
        new_grade.course = Course::get_course_by_id(course_id);
        new_grade.grade = grade;
        GRADES.push_back(new_grade);
        cout << "The grade " << grade << " was successfully set!" << endl;
    }
};

class FileHandler
{
private:
    static bool file_exists(string file_name)
    {
        std::ifstream file(file_name);
        return file.good();
    }

    static vector<string> split_string(string str, string separator)
    {
        vector<string> result;
        size_t start = 0;
        size_t end = str.find(separator);
        while (end != string::npos)
        {
            result.push_back(str.substr(start, end - start));
            start = end + separator.length();
            end = str.find(separator, start);
        }
        result.push_back(str.substr(start, end));
        return result;
    }

    static string slice_string(string str, int index)
    {
        return str.substr(index, str.length() - 3);
    }

    static bool is_substring(string str1, string str2)
    {
        return str1.find(str2) != string::npos;
    }

public:
    static string turn_student_info_into_string(Student *std)
    {
        string data = "";
        string separator = "     ";

        data += "name : " + std->get_name() + separator;
        data += "id : " + to_string(std->get_student_id()) + separator;
        data += "age : " + to_string(std->get_age()) + separator;
        data += "phone number : " + to_string(std->get_phone_number()) + separator;
        if (std->get_avg_grade() != -1)
            data += "average grade : " + to_string(std->get_avg_grade()) + separator;
        if (std->get_enrolled_courses().size() > 0)
            data += "courses : ";
        for (auto i : std->get_enrolled_courses())
        {
            data += to_string(i->get_id()) + "  ,  ";
        }
        return data.substr(0, data.length() - 5);
    }

    static string turn_course_info_into_string(Course *crs)
    {
        string data = "";
        string separator = "     ";

        data += "title : " + crs->get_title() + separator;
        data += "id : " + to_string(crs->get_id()) + separator;
        data += "capacity : " + to_string(crs->get_capacity()) + separator;
        data += "student count : " + to_string(crs->student_count()) + separator;
        data += "is mandatory : " + to_string(crs->get_is_mandatory()) + separator;
        if (crs->get_description().length() > 0)
            data += "description : " + crs->get_description() + separator;
        if (crs->get_attending_students().size() > 0)
            data += "students : ";
        for (auto i : crs->get_attending_students())
        {
            data += to_string(i->get_student_id()) + "  ,  ";
        }
        return data.substr(0, data.length() - 5);
    }

    static string turn_grade_info_into_string(Grade grade)
    {
        string data = "";
        string separator = "     ";

        data += "student id : " + to_string(grade.student->get_student_id()) + separator;
        data += "course id : " + to_string(grade.course->get_id()) + separator;
        data += "grade : " + to_string(grade.grade);

        return data;
    }

    static void get_student_from_file(string str)
    {
        vector<string> parts = split_string(str, "     ");

        string name = slice_string(parts[0], 7);
        int id = stoi(slice_string(parts[1], 5));
        int age = stoi(slice_string(parts[2], 6));
        long int phone = stol(slice_string(parts[3], 15));

        // float avg_grade = -1;
        vector<int> courses;
        // if (parts.size() >= 5   &&  is_substring(parts[4], "average grade : "))
        //     avg_grade = stof(slice_string(parts[4], 16));

        if (parts.size() >= 5 && is_substring(parts[4], "courses : "))
        {
            string s = slice_string(parts[4], 10);
            for (auto i : split_string(s, "  ,  "))
            {
                courses.push_back(stoi(i));
            }
        }

        if (parts.size() >= 6 && is_substring(parts[5], "courses : "))
        {
            string s = slice_string(parts[5], 10);
            for (auto i : split_string(s, "  ,  "))
            {
                courses.push_back(stoi(i));
            }
        }

        Student *std = new Student(name, id, age, phone);

        for (auto i : courses)
        {
            // Enrollment::enroll_to_course(std->get_student_id(), i);
            Course *crs = Course::get_course_by_id(i);
            std->add_course(crs);
            crs->add_student(std);
        }
    }

    static void get_course_from_file(string str)
    {
        vector<string> parts = split_string(str, "     ");

        string title = slice_string(parts[0], 8);
        int id = stoi(slice_string(parts[1], 5));
        int capacity = stoi(slice_string(parts[2], 11));
        // int std_c = stoi(slice_string(parts[3], 16));
        bool is_mandatory = stoi(slice_string(parts[4], 15));
        string desc = "";
        vector<int> students;

        if (parts.size() >= 6 && is_substring(parts[5], "description : "))
        {
            desc = slice_string(parts[5], 14);
        }

        else if (parts.size() >= 6 && is_substring(parts[5], "students : "))
        {
            string s = slice_string(parts[5], 11);
            for (auto i : split_string(s, "  ,  "))
            {
                students.push_back(stoi(i));
            }
        }

        if (parts.size() >= 7 && is_substring(parts[6], "students : "))
        {
            string s = slice_string(parts[6], 11);
            for (auto i : split_string(s, "  ,  "))
            {
                students.push_back(stoi(i));
            }
        }

        Course *crs = new Course(id, title, capacity, is_mandatory);
        crs->set_description(desc);

        // for (auto i : students) {
        //     crs->add_student(Student::get_student_by_id(i));
        // }
    }

    static void get_grade_from_file(string str)
    {
        vector<string> parts = split_string(str, "     ");

        int std_id, crs_id;
        float grade;

        std_id = stoi(slice_string(parts[0], 13));
        crs_id = stoi(slice_string(parts[1], 12));
        grade = stof(slice_string(parts[2], 8));

        Grade g;
        g.grade = grade;
        g.student = Student::get_student_by_id(std_id);
        g.course = Course::get_course_by_id(crs_id);

        GRADES.push_back(g);
    }

    static void read_all_students_from_file()
    {
        if (!file_exists(::students_file))
        {
            cout << "Student's file does not exist!\n"
                 << endl;
            return;
        }
        fstream file(::students_file, ios::in);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                get_student_from_file(line);
            }
            file.close();
        }
        else
        {
            cerr << "Unable to open \"" << ::students_file << "\" ." << endl;
        }
    }

    static void read_all_courses_from_file()
    {
        if (!file_exists(::courses_file))
        {
            cout << "Course's file does not exist!\n"
                 << endl;
            return;
        }
        fstream file(::courses_file, ios::in);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                get_course_from_file(line);
            }
            file.close();
        }
        else
        {
            cerr << "Unable to open \"" << ::courses_file << "\" ." << endl;
        }
    }

    static void read_all_grades_form_file()
    {
        if (!file_exists(::grades_file))
        {
            cout << "Grades file does not exist!\n"
                 << endl;
            return;
        }
        fstream file(::grades_file, ios::in);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                get_grade_from_file(line);
            }
            file.close();
        }
        else
        {
            cerr << "Unable to open \"" << ::grades_file << "\" ." << endl;
        }
    }

    static void update_students_file()
    {
        fstream file(::students_file, ios::out);
        if (file.is_open())
        {
            for (auto i : STUDENTS)
            {
                file << turn_student_info_into_string(i) << '\n';
            }
            file.close();
        }
        else
            cerr << "Unable to open \"" << ::students_file << "\" ." << endl;
    }

    static void update_courses_file()
    {
        fstream file(::courses_file, ios::out);
        if (file.is_open())
        {
            for (auto i : COURSES)
            {
                file << turn_course_info_into_string(i) << '\n';
            }
            file.close();
        }
        else
            cerr << "Unable to open \"" << ::courses_file << "\" ." << endl;
    }

    static void update_grades_file()
    {
        fstream file(::grades_file, ios::out);
        if (file.is_open())
        {
            for (auto i : GRADES)
            {
                file << turn_grade_info_into_string(i) << '\n';
            }
            file.close();
        }
        else
            cerr << "Unable to open \"" << ::grades_file << "\" ." << endl;
    }
};

void Student::drop_course(Course *crs)
{
    // for (auto i : this->courses)
    // {
    //     if (i->get_id() == crs->get_id())
    //     {
    //         this->courses.erase(remove(this->courses.begin(), this->courses.end(), crs), this->courses.end());
    //         cout << "Student \"" << this->get_name() << "\" dropped out of course \"" << crs->get_title() << "\" ." << endl;
    //         return;
    //     }
    // }
    // cout << "Student \"" << this->get_name() << "\" is not attending that course!" << endl;
    this->courses.erase(remove(this->courses.begin(), this->courses.end(), crs), this->courses.end());
}

float Student::get_avg_grade()
{
    vector<Grade> grades = Enrollment::get_student_grades(this->get_student_id());

    if (grades.size() == 0)
        return -1;

    float avg = 0;
    for (auto i : grades)
    {
        avg += i.grade;
    }
    avg /= grades.size();
    return avg;
}

void Student::show_info()
{
    cout << "\nStudent name : " << this->name << endl;
    cout << "Student id : " << this->id << endl;
    cout << "Student age : " << this->age << endl;
    cout << "Student phone number : " << this->phone_number << endl;
    cout << "Courses : " << endl;
    for (auto i : this->courses)
    {
        cout << "   " << i->get_title() << endl;
    }
    if (this->get_avg_grade() != -1)
        cout << "Student average grade : " << this->get_avg_grade() << endl;
}

void Course::show_info()
{
    cout << "\nCourse title : " << this->title << endl;
    if (this->description.length() > 0)
        cout << "Course description : " << this->description << endl;
    cout << "Course capacity : " << this->capacity << endl;
    cout << "Is mandatory : " << this->is_mandatory << endl;
    cout << "Attending students count : " << this->student_count() << endl;
    cout << "Students : " << endl;
    for (auto i : this->students)
    {
        cout << "   " << i->get_name() << " (id " << i->get_student_id() << ") " << endl;
    }
}

void Student::remove_student()
{
    for (auto i : this->courses)
    {
        i->drop_student(this);
    }
    this->remove_grades();
    STUDENTS.erase(remove(STUDENTS.begin(), STUDENTS.end(), this), STUDENTS.end());
    delete this;
}

void Course::remove_course()
{
    for (auto i : this->students)
    {
        i->drop_course(this);
    }
    this->remove_grades();
    COURSES.erase(remove(COURSES.begin(), COURSES.end(), this), COURSES.end());
    delete this;
}

bool Grade::operator==(const Grade &other) const
{
    return student->get_student_id() == other.student->get_student_id() && course->get_id() == other.course->get_id();
}

namespace AdminFunctions
{
    void list_of_students();
    void list_of_courses();
    void see_student_profile();
    void see_course_datials();
    void register_new_student();
    void register_new_course();
    void change_student_profile();
    void change_course_datails();
    void enroll_student_to_course();
    void drop_student_out_of_course();
    void register_grades();
    void see_students_grades();
    void see_course_grades();
    void remove_student();
    void remove_course();
};

namespace StudentFunctions
{
    void see_student_profile(int id);
    void see_students_grades(int id);
    void enroll_to_course(int id);
    void change_student_profile(int id);
};

void clean_memory();

main()
{
    FileHandler::read_all_courses_from_file();
    FileHandler::read_all_students_from_file();
    FileHandler::read_all_grades_form_file();

    cout << "===================== Welcome ======================" << endl;

    int status, cmnd;

    do
    {
        cout << "Enter 1 if you are a student" << endl;
        cout << "Enter 2 if you are an admin" << endl;
        cout << ">>> ";
        cin >> status;
        if (status != 1 && status != 2)
            cout << status << " is an unvalid answer! Try again." << endl;
    } while (status != 1 && status != 2);

    int std_id; // only used if user is a student
    if (status == 1)
    {
        do
        {
            cout << "Enter your student id : ";
            cin >> std_id;
            if (Student::id_is_unique(std_id))
                cout << "\nNo student with id " << std_id << " exists! Try again.\n"
                     << endl;
            else
                cout << "\nWelcome " << Student::get_student_by_id(std_id)->get_name() << endl;
        } while (Student::id_is_unique(std_id));
    }

    switch (status)
    {
    case 1:
        do
        {
            cout << "\nEnter 1 to see your profile" << endl;
            cout << "Enter 2 to see your grade in each course" << endl;
            cout << "Enter 3 to see a list of all available courses" << endl;
            cout << "Enter 4 to see a course's details" << endl;
            cout << "Enter 5 to enroll to a new course" << endl;
            cout << "Enter 6 to change your profile" << endl;
            cout << "Enter 0 to exit" << endl;

            cout << ">>> ";
            cin >> cmnd;

            switch (cmnd)
            {
            case 1:
                StudentFunctions::see_student_profile(std_id);
                break;

            case 2:
                StudentFunctions::see_students_grades(std_id);
                break;

            case 3:
                AdminFunctions::list_of_courses();
                break;

            case 4:
                AdminFunctions::see_course_datials();
                break;

            case 5:
                StudentFunctions::enroll_to_course(std_id);
                break;

            case 6:
                StudentFunctions::change_student_profile(std_id);
                break;

            case 0:
                cout << "\nYou exited\n";
                break;

            default:
                cout << '\n'
                     << cmnd << " is not a valid command! Try again." << endl;
                break;
            }

        } while (cmnd);
        break;

    case 2:
        do
        {
            cout << "\nEnter 1 to see a list of all students" << endl;
            cout << "Enter 2 to see a list of all courses" << endl;
            cout << "Enter 3 to see a student's profile" << endl;
            cout << "Enter 4 to see a course's details" << endl;
            cout << "Enter 5 to register a new student" << endl;
            cout << "Enter 6 to register a new course" << endl;
            cout << "Enter 7 to change a student's profile" << endl;
            cout << "Enter 8 to change a course's details" << endl;
            cout << "Enter 9 to enroll a student to a course" << endl;
            cout << "Enter 10 to drop a student out of a course" << endl;
            cout << "Enter 11 to register grades" << endl;
            cout << "Enter 12 to see a student's grades" << endl;
            cout << "Enter 13 to see a course's grades" << endl;
            cout << "Enter 14 to remove a student" << endl;
            cout << "Enter 15 to remove a course" << endl;
            cout << "Enter 0 to exit" << endl;

            cout << ">>> ";
            cin >> cmnd;

            switch (cmnd)
            {
            case 1:
                AdminFunctions::list_of_students();
                break;

            case 2:
                AdminFunctions::list_of_courses();
                break;

            case 3:
                AdminFunctions::see_student_profile();
                break;

            case 4:
                AdminFunctions::see_course_datials();
                break;

            case 5:
                AdminFunctions::register_new_student();
                break;

            case 6:
                AdminFunctions::register_new_course();
                break;

            case 7:
                AdminFunctions::change_student_profile();
                break;

            case 8:
                AdminFunctions::change_course_datails();
                break;

            case 9:
                AdminFunctions::enroll_student_to_course();
                break;

            case 10:
                AdminFunctions::drop_student_out_of_course();
                break;

            case 11:
                AdminFunctions::register_grades();
                break;

            case 12:
                AdminFunctions::see_students_grades();
                break;

            case 13:
                AdminFunctions::see_course_grades();
                break;

            case 14:
                AdminFunctions::remove_student();
                break;

            case 15:
                AdminFunctions::remove_course();
                break;

            case 0:
                cout << "\nYou exited\n";
                break;

            default:
                cout << '\n'
                     << cmnd << " is not a valid command! Try again." << endl;
                break;
            }
        } while (cmnd);
        break;
    }

    clean_memory();

    return 0;
}

void AdminFunctions::list_of_students()
{
    if (STUDENTS.size() == 0)
    {
        cout << "\nThere are no registered students yet!" << endl;
        return;
    }
    cout << '\n';
    for (auto i : STUDENTS)
    {
        cout << "name : " << i->get_name() << "      id : " << i->get_student_id() << endl;
    }
}

void AdminFunctions::list_of_courses()
{
    if (COURSES.size() == 0)
    {
        cout << "\nThere are no registered courses yet!" << endl;
        return;
    }
    cout << '\n';
    for (auto i : COURSES)
    {
        cout << "title : " << i->get_title() << "      id : " << i->get_id() << endl;
    }
}

void AdminFunctions::see_student_profile()
{
    int id;
    cout << "\nEnter student id : ";
    cin >> id;
    if (Student::id_is_unique(id))
    {
        cout << "\nNo student with id " << id << " exists!" << endl;
        return;
    }
    Student *s = Student::get_student_by_id(id);
    cout << '\n';
    s->show_info();
}

void AdminFunctions::see_course_datials()
{
    int id;
    cout << "\nEnter course id : ";
    cin >> id;
    if (Course::id_is_unique(id))
    {
        cout << "\nNo course with id " << id << " exists!" << endl;
        return;
    }
    Course *c = Course::get_course_by_id(id);
    cout << '\n';
    c->show_info();
}

void AdminFunctions::change_student_profile()
{
    int id;
    cout << "\nEnter student id : ";
    cin >> id;
    if (Student::id_is_unique(id))
    {
        cout << "\nNo student with id " << id << " exists!" << endl;
        return;
    }
    Student *s = Student::get_student_by_id(id);
    string name;
    int new_id, age;
    long int phone;
    cout << "You are changing " << s->get_name() << "'s profile." << endl;
    cout << "Enter new name : ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter new id (must be unique, or the same id) : ";
    cin >> new_id;
    if (!Student::id_is_unique(new_id) && new_id != id)
    {
        cout << "\nAnother student with id " << new_id << " exists!" << endl;
        return;
    }
    cout << "Enter new age : ";
    cin >> age;
    cout << "Enter new phone number : ";
    cin >> phone;
    s->set_name(name);
    s->set_student_id(new_id);
    s->set_age(age);
    s->set_phone_number(phone);
    FileHandler::update_students_file();

    cout << "\nChanges were registered successfully!" << endl;
}

void AdminFunctions::change_course_datails()
{
    int id;
    cout << "\nEnter course id : ";
    cin >> id;
    if (Course::id_is_unique(id))
    {
        cout << "\nNo course with id " << id << " exists!" << endl;
        return;
    }
    Course *c = Course::get_course_by_id(id);
    string title, desc;
    int new_id, capacity;

    cout << "You are editting the datials of \"" << c->get_title() << "\"." << endl;

    cout << "Enter new title : ";
    cin.ignore();
    getline(cin, title);

    cout << "Enter new id (must be unique, or the same) : ";
    cin >> new_id;
    if (!Course::id_is_unique(new_id) && new_id != id)
    {
        cout << "\nAnother course with id " << new_id << " exists!" << endl;
        return;
    }

    cout << "Enter new capacity (greater than or equal to " << MIN_COURSE_CAPACITY << ") : ";
    cin >> capacity;
    if (capacity < MIN_COURSE_CAPACITY)
    {
        cout << "\nCapacity of a course must be greater than or equal to " << MIN_COURSE_CAPACITY << endl;
        return;
    }

    cout << "Enter new description (not needed) : ";
    cin.ignore();
    getline(cin, desc);

    c->set_title(title);
    c->set_id(new_id);
    c->set_capacity(capacity);
    c->set_description(desc);
    FileHandler::update_courses_file();

    cout << "\nChanges were registered successfully!" << endl;
}

void AdminFunctions::register_new_student()
{
    string name;
    int id, age;
    long int phone;

    cout << "\nRegistering a new student\n"
         << endl;

    cout << "Enter student name : ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter student id (must be unique) : ";
    cin >> id;
    if (!Student::id_is_unique(id))
    {
        cout << "The id " << id << " is not unique!" << endl;
        return;
    }
    cout << "Enter student age : ";
    cin >> age;
    cout << "Enter student phone number (needed) : ";
    cin >> phone;

    if (phone < 1000) {
        cout << "\nPhone number is too short!" << endl;
        return;
    }

    Student *s = new Student(name, id, age, phone);
    FileHandler::update_students_file();

    cout << "\nThe new student \"" << name << "\" is registered!\n"
         << endl;
}

void AdminFunctions::register_new_course()
{
    string title, desc;
    int id, capacity;
    char mandatory;
    bool is_mandatory;

    cout << "\nRegistering a new course\n"
         << endl;

    cout << "Enter course title : ";
    cin.ignore();
    getline(cin, title);

    cout << "Enter course id (must be unique) : ";
    cin >> id;
    if (!Course::id_is_unique(id))
    {
        cout << "The id " << id << " is not unique!" << endl;
        return;
    }

    cout << "Enter course capacity (greater than or equal to " << MIN_COURSE_CAPACITY << ") : ";
    cin >> capacity;
    if (capacity < MIN_COURSE_CAPACITY)
    {
        cout << "\nCapacity of a course must be greater than or equal to " << MIN_COURSE_CAPACITY << endl;
        return;
    }

    cout << "Is this course mandatory ? (y/n) : ";
    cin >> mandatory;
    if (mandatory == 'y' || mandatory == 'Y')
        is_mandatory = true;
    else if (mandatory == 'n' || mandatory == 'N')
        is_mandatory = false;
    else
    {
        cout << "Answer not valid!" << endl;
        return;
    }

    cout << "Enter course description (not needed) : ";
    cin.ignore();
    getline(cin, desc);

    Course *c = new Course(id, title, capacity, is_mandatory);
    c->set_description(desc);
    FileHandler::update_courses_file();

    cout << "\nThe new student \"" << title << "\" is registered!\n"
         << endl;
}

void AdminFunctions::enroll_student_to_course()
{
    int std_id, crs_id;
    cout << "\nEnrollment\n"
         << endl;
    cout << "Enter student id : ";
    cin >> std_id;
    if (Student::id_is_unique(std_id))
    {
        cout << "No student with id " << std_id << " exists!" << endl;
        return;
    }
    cout << "Enter course id : ";
    cin >> crs_id;
    if (Course::id_is_unique(crs_id))
    {
        cout << "No course with id " << crs_id << " exists!" << endl;
        return;
    }
    Enrollment::enroll_to_course(std_id, crs_id);
    FileHandler::update_students_file();
    FileHandler::update_courses_file();
}

void AdminFunctions::drop_student_out_of_course()
{
    int std_id, crs_id;
    cout << "\nDropping out\n"
         << endl;
    cout << "Enter student id : ";
    cin >> std_id;
    if (Student::id_is_unique(std_id))
    {
        cout << "No student with id " << std_id << " exists!" << endl;
        return;
    }
    cout << "Enter course id : ";
    cin >> crs_id;
    if (Course::id_is_unique(crs_id))
    {
        cout << "No course with id " << crs_id << " exists!" << endl;
        return;
    }
    Enrollment::drop_out_of_course(std_id, crs_id);
    FileHandler::update_students_file();
    FileHandler::update_courses_file();
    FileHandler::update_grades_file();
}

void AdminFunctions::register_grades()
{
    int std_id, crs_id;
    float grade;

    cout << "\nRegistering grades\n"
         << endl;

    cout << "Enter student id : ";
    cin >> std_id;
    if (Student::id_is_unique(std_id))
    {
        cout << "\nNo student with id " << std_id << " exists!" << endl;
        return;
    }

    cout << "Enter course id : ";
    cin >> crs_id;
    if (Course::id_is_unique(crs_id))
    {
        cout << "\nNo course with id " << crs_id << " exists!" << endl;
        return;
    }

    cout << "You are registering a new grade for \"" << Student::get_student_by_id(std_id)->get_name() << "\" in course \"" << Course::get_course_by_id(crs_id)->get_title() << "\" ." << endl;

    cout << "Enter grade (between 0 and 20) : ";
    cin >> grade;
    if (grade < 0 || grade > 20)
    {
        cout << "\nInvalid grade!" << endl;
        return;
    }
    Enrollment::set_naw_grade(std_id, crs_id, grade);
    FileHandler::update_students_file();
    FileHandler::update_courses_file();
    FileHandler::update_grades_file();
}

void AdminFunctions::see_students_grades()
{
    int id;
    cout << "\nEnter student id : ";
    cin >> id;

    if (Student::id_is_unique(id))
    {
        cout << "No student with id " << id << " exists!" << endl;
        return;
    }

    vector<Grade> grades = Enrollment::get_student_grades(id);

    if (grades.size() == 0)
    {
        cout << "No grades are registered for this student yet." << endl;
        return;
    }

    cout << "Grades of student \"" << Student::get_student_by_id(id)->get_name() << "\" :" << endl;

    for (auto i : grades)
    {
        cout << "course : " << i.course->get_title() << "     grade : " << i.grade << endl;
    }
}

void AdminFunctions::see_course_grades()
{
    int id;
    cout << "\nEnter course id : ";
    cin >> id;

    if (Course::id_is_unique(id))
    {
        cout << "No course with id " << id << " exists!" << endl;
        return;
    }

    vector<Grade> grades = Enrollment::get_course_grades(id);

    if (grades.size() == 0)
    {
        cout << "No grades are registered for this course yet." << endl;
        return;
    }

    cout << "Grades of course \"" << Course::get_course_by_id(id)->get_title() << "\" :" << endl;

    for (auto i : grades)
    {
        cout << "student : " << i.student->get_name() << "     grade : " << i.grade << endl;
    }
}

void AdminFunctions::remove_student()
{
    int id;

    cout << "\nRemoving a student\n"
         << endl;

    cout << "Enter student's id : ";
    cin >> id;
    if (Student::id_is_unique(id))
    {
        cout << "\nNo student with this id exists!\n"
             << endl;
        return;
    }

    Student *s = Student::get_student_by_id(id);
    char are_you_sure;

    cout << "Are you sure you want to remove student \"" << s->get_name() << "\" ? (y/n) : ";
    cin >> are_you_sure;

    if (are_you_sure=='n' || are_you_sure=='N') {
        cout << "\nDeleting process canceled" << endl;
        return;
    }
    else if (are_you_sure!='y' && are_you_sure!='Y') {
        cout << are_you_sure << " is not a valid answer! Deleting process canceled." << endl;
        return;
    }

    cout << "\nThe student \"" << s->get_name() << "\" was successfully removed!\n"
         << endl;
    s->remove_student();
    FileHandler::update_students_file();
    FileHandler::update_courses_file();
    FileHandler::update_grades_file();
}

void AdminFunctions::remove_course()
{
    int id;

    cout << "\nRemoving a course\n"
         << endl;

    cout << "Enter course's id : ";
    cin >> id;
    if (Course::id_is_unique(id))
    {
        cout << "\nNo course with this id exists!\n"
             << endl;
        return;
    }

    Course *c = Course::get_course_by_id(id);
    char are_you_sure;

    cout << "Are you sure you want to remove course \"" << c->get_title() << "\" ? (y/n) : ";
    cin >> are_you_sure;

    if (are_you_sure=='n' || are_you_sure=='N') {
        cout << "\nDeleting process canceled" << endl;
        return;
    }
    else if (are_you_sure!='y' && are_you_sure!='Y') {
        cout << are_you_sure << " is not a valid answer. Deleting process canceled." << endl;
        return;
    }

    cout << "\nThe course \"" << c->get_title() << "\" was successfully removed!\n"
         << endl;
    c->remove_course();
    FileHandler::update_courses_file();
    FileHandler::update_students_file();
    FileHandler::update_grades_file();
}

void StudentFunctions::see_student_profile(int id)
{
    Student *s = Student::get_student_by_id(id);
    s->show_info();
}

void StudentFunctions::see_students_grades(int id)
{
    cout << "\nYour grades" << endl;
    vector<Grade> grades = Enrollment::get_student_grades(id);
    if (grades.size() == 0)
    {
        cout << "No grades are registered for you yet." << endl;
        return;
    }
    for (auto i : grades)
    {
        cout << "course : " << i.course->get_title() << "     grade : " << i.grade << endl;
    }
}

void StudentFunctions::enroll_to_course(int id)
{
    int crs_id;
    cout << "\nEnter course id : ";
    cin >> crs_id;

    Enrollment::enroll_to_course(id, crs_id);
    FileHandler::update_students_file();
    FileHandler::update_courses_file();
}

void StudentFunctions::change_student_profile(int id)
{
    string name;
    int age;
    long int phone;

    cout << "\nEnter new name : ";
    cin.ignore();
    getline(cin, name);
    if (name.length() == 0)
    {
        cout << "Invalid name!" << endl;
        return;
    }

    cout << "Enter new age : ";
    cin >> age;

    cout << "Enter new phone number (needed) : ";
    cin >> phone;

    Student *s = Student::get_student_by_id(id);
    s->set_name(name);
    s->set_age(age);
    s->set_phone_number(phone);

    cout << "\nYou successfully changed your profile!" << endl;

    FileHandler::update_students_file();
}

void clean_memory()
{
    for (auto i : STUDENTS)
    {
        i->remove_student();
    }

    for (auto i : COURSES)
    {
        i->remove_course();
    }
}