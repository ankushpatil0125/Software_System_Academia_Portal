#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
char *authenticateStudent(int clientSocket)
{
    struct student my_student, temp;
    int openFD = open("student_database.txt", O_RDONLY, 0644); // Open in read-only mode

    if (openFD == -1)
    {
        perror("Error opening file");
        return "0"; // Return NULL to indicate an error
    }

    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare a buffer for sending data
    send(clientSocket, "Enter Student Username(ID): ", strlen("Enter Student Username(ID): "), 0);
    int readResult = read(clientSocket, my_student.loginId, sizeof(my_student.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Student username from server", strlen("Error receiving Student username from server"), 0);
        return "0"; // Return NULL to indicate an error
    }
    my_student.loginId[readResult] = '\0';

    send(clientSocket, "Enter Student Password: ", strlen("Enter Student Password: "), 0);
    readResult = read(clientSocket, my_student.password, sizeof(my_student.password) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Student password from server", strlen("Error receiving Student password from server"), 0);
        return "0"; // Return NULL to indicate an error
    }
    my_student.password[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    // Loop to search for the faculty in the file
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(my_student.loginId, temp.loginId) == 0 && customStrCmp(my_student.password, temp.password) == 0)
        {
            // Authentication succeeded, dynamically allocate memory for the loginId and return it
            char *loginIdCopy = (char *)malloc(strlen(my_student.loginId) + 1);
            if (loginIdCopy != NULL)
            {
                strcpy(loginIdCopy, my_student.loginId);
                close(openFD);
                return loginIdCopy;
            }
        }
    }
    // Authentication failed
    close(openFD);
    return "0"; // Return NULL to indicate an error
}

int viewAllCourses(int clientSocket)
{
    int openFD = open("course_database.txt", O_RDONLY);
    if (openFD == -1)
    {
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    struct Course temp;
    char buff[1024];
    bool flag = false;
    lseek(openFD, 0, SEEK_SET);
    send(clientSocket, "..........Available Courses...........\n", strlen("..........Available Courses...........\n"), 0);
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(temp.active, "1") == 0)
        {
            sprintf(buff, "Course Id: %s\nProffessor Name: %s\nSubject Name: %s\n........................\n", temp.cid, temp.proffesor_name, temp.name);
            send(clientSocket, buff, strlen(buff), 0);
            flag = true;
        }
    }
    if (flag)
    {
        close(openFD);
        return 1;
    }
    else
    {
        send(clientSocket, "No Course are there to show\n", strlen("No Course are there to show\n"), 0);
        close(openFD);
        return 0;
    }
    close(openFD);
}

int enrollNewCourse(int clientSocket, char *auth)
{
    int val = viewAllCourses(clientSocket);
    if (val == 0)
        return 0;
    struct Course my_course, temp;
    struct Enroll enr, tenr;
    int openFD = open("course_database.txt", O_RDWR);
    int openFD1 = open("enrolled_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644);

    if (openFD == -1)
    {
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    if (openFD1 == -1)
    {
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    send(clientSocket, "Enter Course Id to enroll\n", strlen("Enter Course Id to enroll\n"), 0);
    size_t readResult = read(clientSocket, my_course.cid, sizeof(my_course.cid) - 1);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving faculty ID from server\n", strlen("Error receiving faculty ID from server\n"), 0);
        return 0;
    }
    my_course.cid[readResult] = '\0';
    bool found1 = false;
    bool found2 = false;
    lseek(openFD, 0, SEEK_SET);
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(my_course.cid, temp.cid) == 0 && customStrCmp(temp.active, "1") == 0)
        {
            // Course found in course_database.txt
            found1 = true;
            break;
        }
    }
    if (found1)
    {
       
        strcpy(enr.cid, my_course.cid);
        strcpy(enr.loginId, auth);
        lseek(openFD1, 0, SEEK_SET);
        while (read(openFD1, &tenr, sizeof(tenr)) > 0)
        {
            if (customStrCmp(enr.cid, tenr.cid) == 0 && customStrCmp(enr.loginId, tenr.loginId) == 0)
            {
                found2 = true;
                break;
            }
        }
        // check  already enrolled in this course or not
        if (found2)
        {
            send(clientSocket, "You Already Enrolled in the course\n", strlen("You Already Enrolled in the course\n"), 0);
            close(openFD);
            close(openFD1);
            return 0;
        }
        else
        {
            // Make the entry of stud_id and course_id to enrolled_database.txt
            
            int avai_seats = atoi(temp.available_seats);
            if (avai_seats == 0)
            {
                send(clientSocket, "No more Enrollment in this course(Seats are Full)\n", strlen("No more Enrollment in this course(Seats are Full)\n"), 0);
                close(openFD);
                close(openFD1);
                return 0;
            }
            lseek(openFD1, 0, SEEK_END);
            ssize_t bytes_written = write(openFD1, &enr, sizeof(enr));
            if (bytes_written == -1)
            {
                perror("Error while writing the details");
                close(openFD);
                return 0;
            }
            avai_seats -= 1;
            char buff[4];
            sprintf(buff, "%d", avai_seats);
            strcpy(temp.available_seats, buff);
            lseek(openFD, -sizeof(struct Course), SEEK_CUR);
            write(openFD, &temp, sizeof(temp));
            send(clientSocket, "Enrollment to the subject Successfull\n", strlen("Enrollment to the subject Successfull\n"), 0);
            close(openFD);
            close(openFD1);
            return 1;
        }
    }
    else
    {
        send(clientSocket, "Course Not Found\n", strlen("Course Not Found\n"), 0);
        close(openFD);
        close(openFD1);
        return 0;
    }
}

int dropCourse(int clientSocket, char *auth)
{
    struct Course my_course, temp;
    struct Enroll enr, tenr, tenr1;
    int openFD = open("course_database.txt", O_RDWR);
    int openFD1 = open("enrolled_database.txt", O_RDWR | O_CREAT, 0644);
    if (openFD == -1)
    {
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    if (openFD1 == -1)
    {
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    send(clientSocket, "Enter Course Id to drop the course\n", strlen("Enter Course Id to drop the course\n"), 0);
    size_t readResult = read(clientSocket, my_course.cid, sizeof(my_course.cid) - 1);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving faculty ID from server\n", strlen("Error receiving faculty ID from server\n"), 0);
        return 0;
    }
    my_course.cid[readResult] = '\0';
    bool flag1 = false;
    bool flag2 = false;
    // This is for if anyone use drop first instead of first enrolling
    lseek(openFD1, 0, SEEK_SET);
    while (read(openFD1, &tenr1, sizeof(tenr1)) > 0)
    {
        if (customStrCmp(my_course.cid, tenr1.cid) == 0)
        {
            flag1 = true;
        }
    }
    if (flag1)
    {
        //This is for if someone drop the course and then it came again to drop the course 
        lseek(openFD1, 0, SEEK_SET);
        while (read(openFD1, &tenr1, sizeof(tenr1)) > 0)
        {
            if (customStrCmp(my_course.cid,"-1") == 0 && customStrCmp(tenr1.loginId, auth) == 0)
            {
                flag2 = true;
            }
        }
        if (!flag2)
        {
            //This code do actual dropping the course 
            bool found1 = false;
            bool found2 = false;
            strcpy(enr.cid, my_course.cid);
            strcpy(enr.loginId, auth);
            lseek(openFD1, 0, SEEK_SET);
            while (read(openFD1, &tenr, sizeof(tenr)) > 0)
            {
                if (customStrCmp(enr.cid, tenr.cid) == 0 && customStrCmp(enr.loginId, tenr.loginId) == 0)
                {
                    found2 = true;
                    break;
                }
            }
            if (found2)
            {
                strcpy(tenr.cid, "-1");
                lseek(openFD1, -sizeof(struct Enroll), SEEK_CUR);
                write(openFD1, &tenr, sizeof(tenr));

                // increment the count of available seats in coure_database.txt
                lseek(openFD, 0, SEEK_SET);
                while (read(openFD, &temp, sizeof(temp)) > 0)
                {
                    if ((customStrCmp(my_course.cid, temp.cid) == 0 && customStrCmp(temp.active, "1") == 0))
                    {
                        found1 = true;
                        break;
                    }
                }
                if (found1)
                {
                    int avai_seats = atoi(temp.available_seats);
                    avai_seats += 1;
                    char buff[4];
                    sprintf(buff, "%d", avai_seats);
                    //send(clientSocket, buff, sizeof(buff) - 1, 0);
                    strcpy(temp.available_seats, buff);
                    lseek(openFD, -sizeof(struct Course), SEEK_CUR);
                    write(openFD, &temp, sizeof(temp));
                    send(clientSocket, "Successfully drop the course\n", strlen("Successfully drop the course\n"), 0);
                    close(openFD);
                    close(openFD1);
                    return 1;
                }
                else
                {
                    send(clientSocket, "You haven't enrolled in this course\n", strlen("You haven't enrolled in this course\n"), 0);
                    close(openFD);
                    close(openFD1);
                    return 0;
                }
            }
            else
            {
                send(clientSocket, "You haven't enrolled in this course\n", strlen("You haven't enrolled in this course\n"), 0);
                close(openFD);
                close(openFD1);
                return 0;
            }
        }
        else
        {
            send(clientSocket, "Enroll First!You haven't enrolled in this course\n", strlen("Enroll First!You haven't enrolled in this course\n"), 0);
            close(openFD1);
            return 0;
        }
    }
    else
    {
        send(clientSocket, "Cannot Drop the course! First enrolled in this course\n", strlen("Cannot Drop the course! First enrolled in this course\n"), 0);
        close(openFD);
        close(openFD1);
        return 0;
    }
}

int viewEnrolledCourseDetails(int clientSocket, char *auth)
{
    int openFD = open("enrolled_database.txt",O_RDONLY);
    if(openFD==-1){
        perror("Error in opening the file");
        close(openFD);
        return 0;
    }
    bool found=false;
    struct Enroll enr,tenr;
    strcpy(enr.loginId,auth);
    lseek(openFD,0,SEEK_SET);
    while(read(openFD,&tenr,sizeof(tenr)) > 0){
        if(customStrCmp(enr.loginId,tenr.loginId) == 0){
            if(customStrCmp(tenr.cid,"-1") == 0){
                continue;
            }
            else{
                strcpy(enr.cid,tenr.cid);
                int openFD1=open("course_database.txt",O_RDONLY);
                if(openFD1 == -1){
                    perror("Error while opening the file");
                    close(openFD);
                    close(openFD1);
                    continue;
                }
                struct Course my_course,temp;
                strcpy(my_course.cid,enr.cid);
                lseek(openFD1,0,SEEK_SET);
                char buff[1024];
                while(read(openFD1,&temp,sizeof(temp)) > 0){
                    if(customStrCmp(my_course.cid,temp.cid)==0 && customStrCmp(temp.active,"1")==0){
                        //if(customStrCmp(temp.available_seats ,"0") != 0){
                            sprintf(buff,"\nProffessor Name: %s\nCourse Id: %s\nSubject Name: %s\nDepartment: %s\nAvailable Seats: %s\n",temp.proffesor_name,temp.cid,temp.name,temp.dept,temp.available_seats);
                            send(clientSocket,buff,strlen(buff),0);
                            found=true;
                            break;
                        //}
                    }
                    
                }
            }
        }

    }
    if(!found){
        send(clientSocket,"No Enrollment you have done yet\n",strlen("No Enrollment you have done yet\n"),0);
        close(openFD);
        return 0;
    }

    
}

int changeStudentPassword(int clientSocket)
{
    struct student my_student, temp;
    int openFD = open("student_database.txt", O_RDWR, 0644); // Open in read-only mode
    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Student Username ID for updation of password ", strlen("Enter Student Username ID for updation of password: "), 0);
    int readResult = read(clientSocket, my_student.loginId, sizeof(my_student.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Student ID from server", strlen("Error receiving Student ID from server"), 0);
        return 0;
    }
    my_student.loginId[readResult] = '\0';
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(my_student.loginId, temp.loginId) == 0)
        { // Compare the student IDs
            found = true;
            break;
        }
    }
    if (found)
    {
        send(clientSocket, "Enter Password to update: ", strlen("Enter Password to update: "), 0);
        readResult = read(clientSocket, temp.password, sizeof(temp.password) - 1);
        temp.password[readResult] = '\0';

        lseek(openFD, -sizeof(struct student), SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(openFD, &temp, sizeof(temp));               // Overwrite the entire record with the updated data
        close(openFD);

        return 1;
    }
    else
    {
        send(clientSocket, "Student id not found\n", strlen("Student id not found\n"), 0);
        close(openFD);
        return 0;
    }
    return 0;
}

int student_functionality(int clientSocket)
{
    char *auth = authenticateStudent(clientSocket);
    if (strcmp(auth, "0") != 0)
    {
        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client
        int a = 1;
        while (1)
        {
            if (a == 1)
            {
                send(clientSocket, "Login Successfully\n", strlen("Login Successfully\n"), 0);
                a = 0;
            }
            send(clientSocket, "Welcome to the Student Menu\n", strlen("Welcome to the Student Menu\n"), 0);
            char adminPrompt[] = "\nStudent can Do:\n - 1.View All Courses\n - 2.Enroll(Pick) New Course\n - 3.Drop Course\n - 4.View Enrolled Course Details\n - 5.Change Password\n - 6.Exit from this menu\n";

            send(clientSocket, adminPrompt, strlen(adminPrompt), 0);
            // readBytes store no of bytes read from the client by the server
            ssize_t readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
            if (readBytes == -1)
            {
                perror("Error in the choice you provided");
                return false;
            }
            int choice = atoi(readbuff);
            // send(clientSocket,readbuff,sizeof(readbuff),0);

            switch (choice)
            {
            case 1:
                if (!viewAllCourses(clientSocket))
                    continue;
                break;
            case 2:
                if (!enrollNewCourse(clientSocket, auth))
                    continue;
                break;
            case 3:
                if (!dropCourse(clientSocket, auth))
                    continue;
                break;
            case 4:
                if (!viewEnrolledCourseDetails(clientSocket, auth))
                    continue;
                break;
            case 5:
                if (!changeStudentPassword(clientSocket))
                    continue;
                break;
            default:
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
}