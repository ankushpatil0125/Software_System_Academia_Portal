struct UserCredentials
{
    char username[100];
    char pass[100];
};
struct UserCredentials adminCredentials = {"Ankush", "0125"};

struct student
{
	char loginId[11];
    char password[20];
	char name[30];
    char age[3];
    char emailAddress[25];
};

struct Faculty
{
	char loginId[11];
    char password[20];
	char name[30];
    char dept[30];
    char designation[30];
    char emailAddress[25];
};
struct Course
{
    char proffesor_id[10];
    char proffesor_name[20];
    char cid[5];
    char name[20];
    char active[2];
    char dept[20];
    char total_no_of_seats[4];
    char credits[4];
    char available_seats[4];
};
struct Enroll{
    char cid[5];
    char loginId[11];
};





