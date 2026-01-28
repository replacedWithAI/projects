#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;
namespace fileSystem = std::filesystem;

struct LabsOrTutorials {
    string activityName;
    vector<string> days;
    vector<string> labTutorialStartTimes;
    vector<int> duration;
};

struct Lectures { //misleadingly, some courses don't have any sort of lecture
    string activityName;
    vector<string> days;
    vector<string> lectureStartTimes;
    vector<int> duration;
    vector<LabsOrTutorials> labOrTutorials;
};

struct Sections {
    string terms;
    string sectionLetter;
    string professor;
    Lectures lectures;
};

struct Course {
    string faculty;
    string dept;
    string courseCode;
    string credits;
    string courseName;
    string campus;
    vector<string> prerequisites;
    vector<Sections> sections;
};

class DepartmentJsonsGetter {
    std::vector<json> departmentJsons;
    std::vector<string> departmentNames;

    private:
    std::vector<fileSystem::path> getDepartmentFiles(const fileSystem::path& directory) {
        std::vector<fileSystem::path> departmentFilePaths; //make sure file directory is setup correctly

        for (auto& entry : fileSystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                departmentFilePaths.push_back(entry.path());
                departmentNames.push_back(entry.path().stem().string());
            }
        }
        return departmentFilePaths;
    }

    private:
    void storeDepartmentFiles(vector<fileSystem::path> departmentFilePaths) {
        int departmentJsonsIndex = 0;
        for (fileSystem::path departmentFilePath : departmentFilePaths) {
            std::ifstream inputStream(departmentFilePath);
            departmentJsons.push_back(json::parse(inputStream));
        }
    }

    public:
    std::vector<json> getDepartmentJsons() {
        //cout << "Number of department JSONs loaded: " << departmentJsons.size() << endl;
        return departmentJsons;
    }

    public:
    std::vector<std::string> getDepartmentNames() {
        return departmentNames;
    }

    DepartmentJsonsGetter(const fileSystem::path& directory) {
        vector<fileSystem::path> departmentFilePaths = getDepartmentFiles(directory);
        storeDepartmentFiles(departmentFilePaths);
    }
};

class RequestedCoursesInterpreter {
    vector<Course*> interpretedCourses;

    private:
    void storeRequestedCourses(std::vector<std::string> requestedCourses) {
        for (string requestedCourse : requestedCourses) {
            Course* currCourse;

            //assert(count(requestedCourse.begin(), requestedCourse.end(), ' ') > 1 && requestedCourse.length() == 9);
            int spaceIndex = requestedCourse.find(' ');
            currCourse->dept = ( toUpperCase(requestedCourse.substr(0, spaceIndex)) );
            currCourse->courseCode = ( requestedCourse.substr(spaceIndex+1, requestedCourse.length() - spaceIndex+1) );
            //cout << currCourse->dept << " " << currCourse->courseCode << "\n";

            interpretedCourses.push_back(currCourse);
        }
    }

    private:
    std::string toUpperCase(std::string requestedCourseDepartment) {
        string upperCaseDepartment;
        for (char letter : requestedCourseDepartment) {
            upperCaseDepartment += toupper(letter);
        }
        return upperCaseDepartment;
    }

    public:
    vector<Course*> getInterpretedCourses() {
        return interpretedCourses;
    }

    RequestedCoursesInterpreter(const std::vector<std::string> requestedCourses) {
        storeRequestedCourses(requestedCourses);
    }
};

class RequestedCourseDataExtractor {
    private:
    std::vector<int> findDepartmentJsonIndices(vector<Course*> interpretedCourses, std::vector<std::string> departmentNames) {
        vector<int> departmentJsonIndices;
        for(int i = 0; i < interpretedCourses.size(); i++) {
            int start = 0, end = departmentNames.size() - 1;

            while (start <= end) {
                int mid = start + (end - start) / 2;
                if (departmentNames[mid] == interpretedCourses[i]->dept) {
                    departmentJsonIndices[i] = mid;
                    break;
                } else if (departmentNames[mid] < interpretedCourses[i]->dept) {
                    start = mid+1;
                } else {
                    end = mid-1;
                }
            }

            //assert(stard <= end);
            if (start > end) {
                throw std::invalid_argument("Department not found");
                departmentJsonIndices[i] = -1;
            }
        }        
        return departmentJsonIndices;
    }

    std::vector<int> findCourseIndices(vector<Course*> interpretedCourses, std::vector<int> departmentJsonIndices, std::vector<json> departmentJsons) {
        vector<int> courseJsonIndices;

        //need to find the index of the course in the json's list of json strings
        for (int i = 0; i < interpretedCourses.size(); i++) {
            for (int numDepartmentCourses = 0; numDepartmentCourses < departmentJsons[departmentJsonIndices[i]]["courses"].size(); numDepartmentCourses++) {

                json currCourseJsonString = departmentJsons[departmentJsonIndices[i]]["courses"][numDepartmentCourses]["key"];
                if (currCourseJsonString["dept"] == interpretedCourses[i]->dept && currCourseJsonString["code"] == interpretedCourses[i]->courseCode) {
                    courseJsonIndices[i] = numDepartmentCourses;
                    break;
                }
            }
        }
        
        return courseJsonIndices;
    }

    std::vector<Course*> extractDepartmentJsonData(std::vector<int> departmentJsonIndices, std::vector<json> departmentJsons, vector<int> courseJsonIndices) {
        DepartmentJsonProcessor processor(departmentJsonIndices, departmentJsons);
        vector<Course*> extractedCourses = processor->
        return extractedCourses;

    }

    class DepartmentJsonProcessor {
        std::vector<int> departmentJsonIndices;
        std::vector<int> courseJsonIndices;
        std::vector<json> departmentJsons;
        std::vector<Course*> interpretedCourses

        private:
        void processCourses() {
            for (int i = 0; i < interpretedCourses.size(); i++) { //unfortunately, cpp doesn't seem to be able to iterate through a structure's elements
                json currCourseJsonString = departmentJsons[departmentJsonIndices[i]]["courses"][courseJsonIndices[i]];
                interpretedCourses[i]->faculty = currCourseJsonString["key"]["faculty"];
                interpretedCourses[i]->credits = currCourseJsonString["key"]["credits"];
                interpretedCourses[i]->courseName = currCourseJsonString["name"];
                interpretedCourses[i]->campus = currCourseJsonString["prereq"];
                interpretedCourses[i]->sections = processSections(i, currCourseJsonString);
            }
        }

        private:
        vector<Sections*> processSections(int currCourseJsonIndex, json currCourseJsonString) {
            vector<Sections*> sections;
            json currSectionJsonString = currCourseJsonString["schedule"];
            
            for (int i = 0; i < currSectionJsonString.size(); i++) { 
                sections[i]->terms = currSectionJsonString["term"];
                sections[i]->sectionLetter = currSectionJsonString["section"];
                sections[i]->lectures = processLectures(currCourseJsonIndex, currSectionJsonString);
            }
            return sections;
        }

        private:
        vector<Lectures*> processLectures(int currCourseJsonIndex, json currSectionJsonString) {
            Lectures* lectures; //1 section has 1 lecture timeslot
            json currLectureJsonString = currSectionJsonString["classes"][0];

            lectures->activityName = currLectureJsonString["name"];
            currSectionJsonString = currLectureJsonString["timeslot"];
            interpretedCourses->campus = currLectureJsonString["campus"]; //terrible organization; better runtime/memory efficiency

            for (int i = 0; i < currLectureJsonString.size(); i++) {
                lectures->days[i] = currLectureJsonString["weekday"];
                lectures->lectureStartTimes[i] = currLectureJsonString["time"];
                lectures->duration[i] = currLectureJsonString["duration"];
            }
            if (currSectionJsonString["classes"].size() > 1) {
                lectures->labsOrTutorials = processLabsOrTutorials(int currCourseJsonIndex, json currSectionJsonString);
            }
        }

        private:
        vector<LabsOrTutorials*> processLabsOrTutorials(int currCourseJsonIndex, json currCourseJsonString) {
            vector<LabsOrTutorials*> labsOrTutorials;

            for (int i = 0; i < currLabTutorialJsonString.size() - 1; i++) { //the first class is the lecture
                json currLabTutorialJsonString = currCourseJsonString["classes"][i+1];
                labsOrTutorials->activityName = currLectureJsonString["name"];

                for (int labLectureTimeslotElements = 0; labLectureTimeslotElements < currLabTutorialJsonString["timeslot"]; labLectureTimeslotElements++) {
                    labsOrTutorials->days[i] = currLabTutorialJsonString[labLectureTimeslotElements]["weekday"];
                    labsOrTutorials->lectureStartTimes[i] = currLabTutorialJsonString[labLectureTimeslotElements]["time"];
                    labsOrTutorials->duration[i] = currLabTutorialJsonString[labLectureTimeslotElements]["duration"];
                }


        }

        DepartmentJsonProcessor(std::vector<int> departmentJsonIndices, std::vector<json> departmentJsons) {

        }

    };

    RequestedCourseDataExtractor(std::vector<Course*> interpretedCourses, std::vector<std::string> departmentNames, std::vector<json> departmentJsons) {
        vector<int> departmentJsonIndices = findDepartmentJsonIndices(interpretedCourses, departmentNames);

    }
};

class scheduleSolver {


};

    int main() {
        DepartmentJsonsGetter departmentJsonsObj("C:\\Users\\subsa\\Downloads\\output_courses_json 2026_01_15\\output_courses_json 2026_01_15");
        vector<json> departmentJsons = departmentJsonsObj.getDepartmentJsons();
        cout << "Number of department JSONs loaded: " << departmentJsons.size() << endl;
        cout << departmentJsons[0]["courses"].size();

        /*string requestedCourse;
        cout << "Enter the course you want to take (e.g., CPSC 210): ";
        cin >> requestedCourse;
        vector<string> requestedCourses;

        while (requestedCourse != "exit") {
            requestedCourses.push_back(requestedCourse);
            cout << "Enter another course or 'exit' to finish: ";
            cin >> requestedCourse;
        }*/

        /*for (const json& departmentJson : departmentJsons) {
            cout << "Department JSON content: " << departmentJson.dump(4) << endl; // Pretty print with indentation
        }*/



        return 0;
    }