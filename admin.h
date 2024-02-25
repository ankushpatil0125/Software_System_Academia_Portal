
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
#include "structure.h"

int authenticateadmin(int clientSocket)
{
    char username[100];
    char pass[100];

    // Send a prompt for the username
    const char *userPrompt = "\nEnter Username of Admin: ";
    send(clientSocket, userPrompt, strlen(userPrompt), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(clientSocket, username, sizeof(username), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (username[bytesRead - 1] == '\n')
    {
        username[bytesRead - 1] = '\0';
    }
    else
    {
        username[bytesRead] = '\0';
    }

    // Send a prompt for the password
    const char *passPrompt = "\nEnter password of the Admin: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);

    // Receive the password from the client
    bytesRead = recv(clientSocket, pass, sizeof(pass), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (pass[bytesRead - 1] == '\n')
    {
        pass[bytesRead - 1] = '\0';
    }
    else
    {
        pass[bytesRead] = '\0';
    }

    // Compare the received username and password with stored credentials
    if (strcmp(username, adminCredentials.username) == 0 &&
        strcmp(pass, adminCredentials.pass) == 0)
    {
        // Authentication successful
        send(clientSocket, "Authentication successful\n", strlen("Authentication successful\n"), 0);
        return true;
    }
    else
    {
        // Authentication failed
        send(clientSocket, "Authentication failed\n", strlen("Authentication failed\n"), 0);
        return false;
    }
}


// Function to send a prompt to the client and receive a response
int sendPromptAndReceiveResponse(int clientSocket, const char *prompt, char *response, size_t responseSize)
{
    send(clientSocket, prompt, strlen(prompt), 0);
    ssize_t bytes_received = recv(clientSocket, response, responseSize - 1, 0);

    if (bytes_received <= 0)
    {
        perror("Error while receving the response from client");
        return -1;
    }

    response[bytes_received] = '\0'; // Null-terminate the received string
    if (response[bytes_received - 1] == '\n')
    {
        response[bytes_received - 1] = '\0';
    }
    return 0; // Success
}

int addStudent(int clientSocket)
{
    struct student stud;
    struct flock lock;
    int openFD = open("student_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644); // Open the file in append mode
    if (openFD == -1)
    {
        perror("Error while openenig the file student_database.txt");
        close(clientSocket);
        return 0;
    }

    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;
    lock.l_len=0;
    lock.l_start=0;
    fcntl(openFD,F_SETLKW,&lock);
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Student Name: ", stud.name, sizeof(stud.name)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Student LoginId: ", stud.loginId, sizeof(stud.loginId)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Student Password: ", stud.password, sizeof(stud.password)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Student Age: ", stud.age, sizeof(stud.age)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Student Email Address: ", stud.emailAddress, sizeof(stud.emailAddress)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &stud, sizeof(stud));

    if (bytes_written == -1)
    {
        perror("Error while writing the details");
        close(openFD);
        close(clientSocket);
        return 0;
    }

    lock.l_type=F_UNLCK;
    fcntl(openFD,F_SETLKW,&lock);
    //sending a success message to the client.
    const char successMessage[] = "Student added successfully...\n";
    send(clientSocket, successMessage, strlen(successMessage), 0);

    close(openFD);
    return 1; // Success
}

int addFaculty(int clientSocket)
{
    struct Faculty faculty;
    struct flock lock;
    int openFD = open("faculty_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644); // Open the file in append mode
    if (openFD == -1)
    {
        perror("Error while opening the file faculty_database.txt ");
        close(clientSocket);
        return 0;
    }

    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;
    lock.l_len=0;
    lock.l_start=0;
    fcntl(openFD,F_SETLKW,&lock);
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty Name: ", faculty.name, sizeof(faculty.name)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty LoginId: ", faculty.loginId, sizeof(faculty.loginId)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty Password: ", faculty.password, sizeof(faculty.password)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty Department: ", faculty.dept, sizeof(faculty.dept)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty Designation: ", faculty.designation, sizeof(faculty.designation)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Faculty Email Address: ", faculty.emailAddress, sizeof(faculty.emailAddress)) == -1)
    {
        close(clientSocket);
        lock.l_type=F_UNLCK;
        fcntl(openFD,F_SETLKW,&lock);
        close(openFD);
        return 0;
    }

    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &faculty, sizeof(faculty));

    if (bytes_written == -1)
    {
        perror("Error while write the details");
        close(openFD);
        close(clientSocket);
        return 0;
    }

    lock.l_type=F_UNLCK;
    fcntl(openFD,F_SETLKW,&lock);
    //send a success message to the client.
    const char successMessage[] = "Faculty added successfully...\n";
    send(clientSocket, successMessage, strlen(successMessage), 0);

    close(openFD);
    return 1; // Success
}


int customStrCmp(const char *str1, const char *str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

int viewStudentDetails(int clientSocket)
{
    struct student my_student, temp;
    int openFD = open("student_database.txt", O_RDONLY, 0644); // Open in read-only mode

    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Student ID: ", strlen("Enter Student ID: "), 0);
    int readResult = read(clientSocket, my_student.loginId, sizeof(my_student.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving student ID from server", strlen("Error receiving student ID from server"), 0);
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
        // Construct the details message
        sprintf(buffer, "...................Student..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                temp.name, temp.age, temp.loginId, temp.password,temp.emailAddress);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        return 1;
    }
    else
    {
        send(clientSocket, "Student not found\n", strlen("Student not found\n"), 0);
        return 0;
    }
    close(openFD);
    return 0; // Close the file after use
}

int viewFacultyDetails(int clientSocket)
{
    struct Faculty my_faculty, temp;
    int openFD = open("faculty_database.txt", O_RDONLY, 0644); // Open in read-only mode

    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Faculty ID: ", strlen("Enter Faculty ID: "), 0);
    int readResult = read(clientSocket, my_faculty.loginId, sizeof(my_faculty.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving faculty ID from server", strlen("Error receiving faculty ID from server"), 0);
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
            break;
        }
    }
    if (found)
    {
        // Construct the details message
        sprintf(buffer, "...................Faculty..................\nName: %s\nDepartment: %s\nDesignation: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                temp.name, temp.dept,temp.designation, temp.loginId, temp.password,temp.emailAddress);
        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        close(openFD);
        return 1;
    }
    else
    {
        send(clientSocket, "Faculty not found\n", strlen("Faculty not found\n"), 0);
        close(openFD);
        return 0;
    }
    close(openFD);
    return 0; // Close the file after use
}

int updateStudentDetails(int clientSocket)
{
    struct student my_student, temp;
    int openFD = open("student_database.txt", O_RDWR, 0644); // Open in read-only mode
    struct flock lock;
    if (openFD == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Student ID for updation of data: ", strlen("Enter Student ID for updation of data: "), 0);
    int readResult = read(clientSocket, my_student.loginId, sizeof(my_student.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving student ID from server", strlen("Error receiving student ID from server"), 0);
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
            
            lock.l_type = F_WRLCK;  // Write (exclusive) lock
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(openFD, 0, SEEK_CUR) - sizeof(struct student); // Position lock at the current record
            lock.l_len = sizeof(struct student);

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
        sprintf(buffer, "...................Student..................\n...................Original Details ..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                temp.name, temp.age, temp.loginId, temp.password,temp.emailAddress);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        
        struct student up_stud;
        strcpy(up_stud.loginId, temp.loginId);

        send(clientSocket, "Enter Student Name to update: ", strlen("Enter Student Name to update: "), 0);
        readResult = read(clientSocket, up_stud.name, sizeof(up_stud.name) - 1);
        up_stud.name[readResult]='\0';

        send(clientSocket, "Enter password to update: ", strlen("Enter password to update: "), 0);
        readResult = read(clientSocket, up_stud.password, sizeof(up_stud.password) - 1);
        up_stud.password[readResult]='\0';

        send(clientSocket, "Enter Age to update: ", strlen("Enter Age to update: "), 0);
        readResult = read(clientSocket, up_stud.age, sizeof(up_stud.age) - 1);
        up_stud.age[readResult]='\0';

        send(clientSocket, "Enter Email Address to update: ", strlen("Enter Email Address to update: "), 0);
        readResult = read(clientSocket, up_stud.emailAddress, sizeof(up_stud.emailAddress) - 1);
        up_stud.emailAddress[readResult]='\0';

        lseek(openFD,-sizeof(struct student),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(openFD,&up_stud,sizeof(up_stud));// Overwrite the entire record with the updated data

        sprintf(buffer, "...................Student..................\n...................Updated Details ..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                up_stud.name, up_stud.age, up_stud.loginId, up_stud.password,up_stud.emailAddress);
        
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
        send(clientSocket, "Student not found\n", strlen("Student not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);
        return 0;
    }
    close(openFD);
    return 0; // Close the file after use
}

int updateFacultyDetails(int clientSocket)
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
    send(clientSocket, "Enter Faculty ID for updation of data: ", strlen("Enter Faculty ID for updation of data: "), 0);
    int readResult = read(clientSocket, my_faculty.loginId, sizeof(my_faculty.loginId) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Faculty ID from server", strlen("Error receiving Faculty ID from server"), 0);
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

        // Construct the details message
        sprintf(buffer, "...................Student..................\n...................Original Details ..................\nName: %s\nDeptartment: %s\nDesignation: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                temp.name, temp.dept,temp.designation, temp.loginId, temp.password,temp.emailAddress);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        
        struct Faculty up_faculty;
        strcpy(up_faculty.loginId, temp.loginId);

        send(clientSocket, "Enter Faculty Name to update: ", strlen("Enter Faculty Name to update: "), 0);
        readResult = read(clientSocket, up_faculty.name, sizeof(up_faculty.name) - 1);
        up_faculty.name[readResult]='\0';

        send(clientSocket, "Enter password to update: ", strlen("Enter password to update: "), 0);
        readResult = read(clientSocket, up_faculty.password, sizeof(up_faculty.password) - 1);
        up_faculty.password[readResult]='\0';

        send(clientSocket, "Enter Department to update: ", strlen("Enter Department to update: "), 0);
        readResult = read(clientSocket, up_faculty.dept, sizeof(up_faculty.dept) - 1);
        up_faculty.dept[readResult]='\0';

        send(clientSocket, "Enter Designation to update: ", strlen("Enter Designation to update: "), 0);
        readResult = read(clientSocket, up_faculty.designation, sizeof(up_faculty.designation) - 1);
        up_faculty.designation[readResult]='\0';

        send(clientSocket, "Enter Email Address to update: ", strlen("Enter Email Address to update: "), 0);
        readResult = read(clientSocket, up_faculty.emailAddress, sizeof(up_faculty.emailAddress) - 1);
        up_faculty.emailAddress[readResult]='\0';

        lseek(openFD,-sizeof(struct Faculty),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(openFD,&up_faculty,sizeof(up_faculty));// Overwrite the entire record with the updated data

        sprintf(buffer, "...................Faculty..................\n...................Updated Details ..................\nName: %s\nDeptartment: %s\nDesignation: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                up_faculty.name, up_faculty.dept,up_faculty.designation, up_faculty.loginId, up_faculty.password,up_faculty.emailAddress);
        
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
        send(clientSocket, "Faculty not found\n", strlen("Faculty not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &lock);
        close(openFD);
        return 0;
    }
    close(openFD);
    return 0; // Close the file after use
}

int admin_functionality(int clientSocket)
{
    if (authenticateadmin(clientSocket))
    {

        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client
        int a;
        while (1)
        {
            if (a == 1)
            {
                send(clientSocket, "Login Successfully\n", strlen("Login Successfully\n"), 0);
                a = 0;
            }
            char adminPrompt[] = "\nAdmin can Do:\n - 1.Add Student\n - 2.Add Faculty\n - 3.View Student Details\n - 4.View Faculty Details\n - 5.Deactivate Student\n - 6. Deactivate Faculty\n - 7. Update Student Details\n - 8. Update Faculty Details\n - 9.Exit from this menu\n";

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
                if (!addStudent(clientSocket))
                    continue;
                break;
            case 2:
                 if (!addFaculty(clientSocket))
                    continue;
                break;
            case 3:
                if (!viewStudentDetails(clientSocket))
                    continue;
                break;
            case 4:
                if (!viewFacultyDetails(clientSocket))
                    continue;
                break;
            case 7:
                if (!updateStudentDetails(clientSocket))
                    continue;
                break;
            case 8:
                if (!updateFacultyDetails(clientSocket))
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
