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
char* authenticateProfessor(int clientSocket) {
    struct Faculty my_faculty, temp;
    int openFD = open("faculty_database.txt", O_RDONLY, 0644); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return "0"; // Return NULL to indicate an error
    }

    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare a buffer for sending data
    send(clientSocket, "Enter Faculty Username(ID): ", strlen("Enter Faculty Username(ID): "), 0);
    int readResult = read(clientSocket, my_faculty.loginId, sizeof(my_faculty.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving faculty username from server", strlen("Error receiving faculty username from server"), 0);
        return "0"; // Return NULL to indicate an error
    }
    my_faculty.loginId[readResult] = '\0';

    send(clientSocket, "Enter Faculty Password: ", strlen("Enter Faculty Password: "), 0);
    readResult = read(clientSocket, my_faculty.password, sizeof(my_faculty.password) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving faculty password from server", strlen("Error receiving faculty password from server"), 0);
        return "0"; // Return NULL to indicate an error
    }
    my_faculty.password[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    // Loop to search for the faculty in the file
    while (read(openFD, &temp, sizeof(temp)) > 0) {
        if (customStrCmp(my_faculty.loginId, temp.loginId) == 0 && customStrCmp(my_faculty.password, temp.password) == 0) {
            // Authentication succeeded, dynamically allocate memory for the loginId and return it
            char* loginIdCopy = (char*)malloc(strlen(my_faculty.loginId) + 1);
            if (loginIdCopy != NULL) {
                strcpy(loginIdCopy, my_faculty.loginId);
                close(openFD);
                return loginIdCopy;
            }
        }
    }
    // Authentication failed
    close(openFD);
    return "0"; // Return NULL to indicate an error
}

int viewOfferingCourses(int clientSocket,char* auth) {
    struct Course my_course, temp;
    int openFD = open("course_database.txt", O_RDONLY, 0644); // Open in read-only mode

    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Course ID: ", strlen("Enter Course ID: "), 0);
    int readResult = read(clientSocket, my_course.cid, sizeof(my_course.cid) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving faculty ID from server", strlen("Error receiving faculty ID from server"), 0);
        return 0;
    }
    my_course.cid[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    // Loop to search for the student in the file
    strcpy(my_course.proffesor_id,auth);
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if((customStrCmp(my_course.cid, temp.cid) == 0) && (customStrCmp(my_course.proffesor_id, temp.proffesor_id) == 0) && (customStrCmp(temp.active,"1")==0))
        { // Compare the student IDs
            found = true;
            break;
        }
    }
    if (found)
    {
        // Construct the details message
        sprintf(buffer, "...................Course Details..................\nName of Subject: %s\nName of Proffesor: %s\nDepartment: %s\nCourse ID: %s\nTotal No of Seats: %s\nAvailable No of Seats %s\nCredits: %s\nActive: %s\n.............................................\n",
                temp.name, temp.proffesor_name,temp.dept,temp.cid, temp.total_no_of_seats, temp.available_seats,temp.credits,temp.active);
        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        close(openFD);
        return 1;
    }
    else
    {
        send(clientSocket, "Course not found\n", strlen("Course not found\n"), 0);
        close(openFD);
        return 0;
    }
    return 0;
}

int addNewCourse(int clientSocket,char* auth) {
    struct Course course;
    int openFD = open("course_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644); // Open the file in append mode
    int openFD1 = open("faculty_database.txt", O_RDONLY | O_CREAT | O_APPEND, 0644); // Open the file in append mode

    if (openFD == -1)
    {
        perror("Error while openenig the file course_database.txt");
        close(clientSocket);
        return 0;
    }
    if (openFD1 == -1)
    {
        perror("Error while openenig the file faculty_database.txt");
        close(clientSocket);
        return 0;
    }
    struct Faculty temp1;
    while (read(openFD1,&temp1,sizeof(temp1)) > 0)
    {
        if(customStrCmp(temp1.loginId,auth)==0)
        {
            strcpy(course.proffesor_name,temp1.name);
            break;
        }
    }
    close(openFD1);
    
    strcpy(course.proffesor_id,auth);
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Subject Name: ", course.name, sizeof(course.name)) == -1)
    {
        //close(clientSocket);
        close(openFD);
        return 0;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Course Id: ", course.cid, sizeof(course.cid)) == -1)
    {
        //close(clientSocket);
        close(openFD);
        return 0;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Department Name: ", course.dept, sizeof(course.dept)) == -1)
    {
        //close(clientSocket);
        close(openFD);
        return 0;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Total No of seats: ", course.total_no_of_seats, sizeof(course.total_no_of_seats)) == -1)
    {
        //close(clientSocket);
        close(openFD);
        return 0;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter No of credits: ", course.credits, sizeof(course.credits)) == -1)
    {
        //close(clientSocket);
        close(openFD);
        return 0;
    }
    strcpy(course.active,"1");
    strcpy(course.available_seats,course.total_no_of_seats);

    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &course, sizeof(course));

    if (bytes_written == -1)
    {
        perror("Error while writing the details");
        close(openFD);
        close(clientSocket);
        return 0;
    }

    // sending a success message to the client.
    const char successMessage[] = "Course added successfully...\n";
    send(clientSocket, successMessage, strlen(successMessage), 0);

    close(openFD);
    return 1; // Success
}

int removeCoursefromCatalog(int clientSocket,char* auth) {
    struct Course my_course, temp;
    int openFD = open("course_database.txt", O_RDWR, 0644); // Open in read-only mode

    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Course ID to Remove: ", strlen("Enter Course ID to Remove: "), 0);
    int readResult = read(clientSocket, my_course.cid, sizeof(my_course.cid) - 1);
    
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving faculty ID from server", strlen("Error receiving faculty ID from server"), 0);
        return 0;
    }
    my_course.cid[readResult] = '\0';
    
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    strcpy(my_course.proffesor_id,auth);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if((customStrCmp(my_course.cid, temp.cid) == 0) && (customStrCmp(my_course.proffesor_id, temp.proffesor_id) == 0)&& customStrCmp(temp.active,"1")==0)
        { // Compare the student IDs
            found = true;
            strcpy(temp.active,"0");
            break;
        }
    }
    
    lseek(openFD,-sizeof(struct Course),SEEK_CUR);
    size_t bytesWrite=write(openFD,&temp,sizeof(temp));
    if(bytesWrite==-1){
        perror("Error while removing course");
        close(openFD);
        return 0;
    }
    if (found)
    {
        send(clientSocket, "Removed the course from catalog\n", strlen("Removed the course from catalog\n"), 0);
        close(openFD);
        return 1;
    }
    else
    {
        send(clientSocket, "Course not found\n", strlen("Course not found\n"), 0);
        close(openFD);
        return 0;
    }
    return 0; 
}

int updateCourseDetails(int clientSocket,char *auth)
{
    struct Course my_course, temp;
    int openFD = open("course_database.txt", O_RDWR, 0644); // Open in read-only mode
    struct flock lock;
    if (openFD == -1)
    {
        perror("Error opening file");
        close(openFD);
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Course ID for updation of Subject details: ", strlen("Enter Course ID for updation of Subject details: "), 0);
    int readResult = read(clientSocket, my_course.cid, sizeof(my_course.cid) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Course ID from server", strlen("Error receiving course ID from server"), 0);
        return 0;
    }
    my_course.cid[readResult] = '\0';
    strcpy(my_course.proffesor_id,auth);
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(my_course.cid, temp.cid) == 0 && customStrCmp(temp.active,"1") == 0 && (customStrCmp(my_course.proffesor_id, temp.proffesor_id) == 0))
        { // Compare the student IDs
            found = true;
            lock.l_type = F_WRLCK;  // Write (exclusive) lock
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(openFD, 0, SEEK_CUR) - sizeof(struct Course); // Position lock at the current record
            lock.l_len = sizeof(struct Course);

            if (fcntl(openFD, F_SETLKW, &lock) == -1)
            {
                perror("Error locking file");
                lock.l_type = F_UNLCK;
                fcntl(openFD, F_SETLK, &lock);
                close(openFD);
                return 0;
            }
            break;
        }
    }
    if (found)
    {

        // Construct the details message
        sprintf(buffer, "...................Course Details..................\nName of Subject: %s\nName of Proffessor: %s\nDepartment: %s\nCourse ID: %s\nTotal No of Seats: %s\nCredits: %s\n.............................................\n",
                temp.name,temp.proffesor_name, temp.dept,temp.cid, temp.total_no_of_seats,temp.credits);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        
        struct Course up_course;
        strcpy(up_course.cid, temp.cid);
        strcpy(up_course.proffesor_name,temp.proffesor_name);
        send(clientSocket, "Enter Name of Subject to update: ", strlen("Enter Name of Subject to update: "), 0);
        readResult = read(clientSocket, up_course.name, sizeof(up_course.name) - 1);
        up_course.name[readResult]='\0';

        send(clientSocket, "Enter Department to update: ", strlen("Enter Department to update: "), 0);
        readResult = read(clientSocket, up_course.dept, sizeof(up_course.dept) - 1);
        up_course.dept[readResult]='\0';

        send(clientSocket, "Enter No of seats to update: ", strlen("Enter No of seats to update: "), 0);
        readResult = read(clientSocket, up_course.total_no_of_seats, sizeof(up_course.total_no_of_seats) - 1);
        up_course.total_no_of_seats[readResult]='\0';

        send(clientSocket, "Enter Credits to update: ", strlen("Enter Credits to update: "), 0);
        readResult = read(clientSocket, up_course.credits, sizeof(up_course.credits) - 1);
        up_course.credits[readResult]='\0';

        lseek(openFD,-sizeof(struct Course),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(openFD,&up_course,sizeof(up_course));// Overwrite the entire record with the updated data

        sprintf(buffer, "...................Course Details..................\nName of Subject: %s\nName of Proffesor: %s\nDepartment: %s\nCourse ID: %s\nTotal No of Seats: %s\nCredits: %s\n.............................................\n",
                up_course.name,up_course.proffesor_name, up_course.dept,up_course.cid, up_course.total_no_of_seats,up_course.credits);
        
        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        //struct flock unlock;
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = lock.l_start;
        lock.l_len = lock.l_len;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);

        return 1;
    }
    else
    {
        send(clientSocket, "CourseId not found\n", strlen("CourseId not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);
        return 0;
    }
    close(openFD);
    return 0; // Close the file after use
}

int changeProfessorPassword(int clientSocket)
{
    struct Faculty my_faculty, temp;
    int openFD = open("faculty_database.txt", O_RDWR, 0644); // Open in read-only mode
    struct flock lock;
    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Proffessor Username ID for updation of password ", strlen("Enter Proffessor Username ID for updation of password: "), 0);
    int readResult = read(clientSocket, my_faculty.loginId, sizeof(my_faculty.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Proffessor ID from server", strlen("Error receiving Proffessor ID from server"), 0);
        return 0;
    }
    my_faculty.loginId[readResult] = '\0';
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0)
    {
        if (customStrCmp(my_faculty.loginId, temp.loginId) == 0)
        { // Compare the student IDs
            found = true;
            lock.l_type = F_WRLCK;  // Write (exclusive) lock
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(openFD, 0, SEEK_CUR) - sizeof(struct Faculty); // Position lock at the current record
            lock.l_len = sizeof(struct Faculty);

            if (fcntl(openFD, F_SETLKW, &lock) == -1)
            {
                perror("Error locking file");
                lock.l_type = F_UNLCK;
                fcntl(openFD, F_SETLK, &lock);
                close(openFD);
                return 0;
            }
            break;
        }
    }
    if (found)
    {
        send(clientSocket, "Enter Password to update: ", strlen("Enter Password to update: "), 0);
        readResult = read(clientSocket, temp.password, sizeof(temp.password) - 1);
        temp.password[readResult]='\0';

        lseek(openFD,-sizeof(struct Faculty),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(openFD,&temp,sizeof(temp));// Overwrite the entire record with the updated data

        //struct flock unlock;
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = lock.l_start;
        lock.l_len = lock.l_len;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);

        return 1;
    }
    else
    {
        send(clientSocket, "Professor id not found\n", strlen("Professor id not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);
        return 0;
    }
    return 0; 
}


int proffesor_functionality(int clientSocket)
{
    char *auth=authenticateProfessor(clientSocket);
    if (strcmp(auth,"0")!=0)
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
            send(clientSocket, "Welcome to the Professor Menu\n", strlen("Welcome to the Professor Menu\n"), 0);
            char adminPrompt[] = "\nProfessor can Do:\n - 1.View Offering Courses\n - 2.Add New Course\n - 3.Remove course from catalog\n - 4.Update Course Details\n - 5.Change Password\n - 6.Exit from this menu\n";

            send(clientSocket, adminPrompt, strlen(adminPrompt), 0);
            // readBytes store no of bytes read from the client by the server
            ssize_t readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
            if (readBytes == -1)
            {
                perror("Error in the choice you provided");
                return 0;
            }
            int choice = atoi(readbuff);
            // send(clientSocket,readbuff,sizeof(readbuff),0);

            switch (choice)
            {
            case 1:
                if (!viewOfferingCourses(clientSocket,auth))
                    continue;
                break;
            case 2:
                if (!addNewCourse(clientSocket,auth))
                    continue;
                break;
            case 3:
                if (!removeCoursefromCatalog(clientSocket,auth))
                    continue;
                break;
            case 4:
                if (!updateCourseDetails(clientSocket,auth))
                    continue;
                break;
            case 5:
                if (!changeProfessorPassword(clientSocket))
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