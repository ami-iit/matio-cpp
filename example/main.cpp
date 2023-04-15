#include <matioCpp/matioCpp.h>
struct testStruct{
    double a;
    double b;
    std::vector<double> c;
};
VISITABLE_STRUCT(testStruct, a, b, c);

int main()
{
    //Create a variable of type int named "hello_element"
    matioCpp::Element<int> i("hello_element");
    i = 8; //Set the value of the element

    // Create a classical vector
    std::vector<double> in = {2.0,4.0,6.0,8.0};
    // Create a matioCpp vector
    matioCpp::Vector<double> vector("test_vector");
    vector = in; //It is possible to copy a standard vector into a matioCpp vector

    std::vector<int> dataVec = {2,4,6,8};
    matioCpp::MultiDimensionalArray<int> mdarray("md_array", {2,2}, dataVec.data()); //It is also possible to save vector data to multi-dimensional arrays, in this case of dimension 2x2. The data is parsed in column-major order

    matioCpp::String testString("string_name"); //Create a string. The name of the string and the content are both "string_name"
    testString = "string content"; //Change the content only to "string_content"

    //Cell arrays are complex data structures including variables of different types. Each variable is stored without a name, and it can be accessed by index
    //Cell arrays can be created given a vector of data to be inserted
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector", 4));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::Vector<double>());
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::CellArray("otherCell"));
    matioCpp::CellArray cell_array("test_cell", {1,2,3}, data); //Create a cell array with dimensions 1x2x3. The data is filled in column-major order
    cell_array[{0,1,0}].asElement<int>() = 7; //edit the "element" integer
    cell_array.setElement(2, vector); //edit the third element we added (the nameless vector of double). Since we needed to resize, we need to use the setElement method

    //Struct is a container of different types of variables, with a given name
    std::vector<matioCpp::Variable> dataStruct;
    dataStruct.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataStruct.emplace_back(matioCpp::Element<int>("element"));
    dataStruct.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataStruct.emplace_back(matioCpp::String("name", "content"));
    dataStruct.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct struct_test("struct", data);

    struct_test["element"].asElement<int>() = 7; //The data can be edited given the name of the field.

    //A struct array is a multidimensional array, where each element is a struct with the same fields (but different data).
    std::vector<matioCpp::Struct> structVector(6, struct_test);
    matioCpp::StructArray struct_array("struct_array", {1,2,3}, structVector); //Create a struct array of dimension 1x2x3

    //
    //make_variable is an handy way to directly convert a generic input to the corresponding matioCpp type
    //

    //If eigen is available, it is possible to easily convert vector and matrices
    Eigen::Matrix3f eigenMatrix;
    eigenMatrix << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    auto toMatioMatrix = matioCpp::make_variable("eigenMatrix", eigenMatrix);

    Eigen::Vector3i eigenVec;
    eigenVec << 2, 4, 6;
    auto toMatioEigenVec = matioCpp::make_variable("eigenVec", eigenVec);

    //Conversions from common types
    std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
    auto toMatioVec = matioCpp::make_variable("stdVec", stdVec);

    std::array<float,3> array = {1.0, 2.0, 3.0};
    auto toMatioArray = matioCpp::make_variable("array", array);

    int classicalArray[] = {1, 2, 3};
    auto toMatioClassic = matioCpp::make_variable("classicalArray", matioCpp::make_span(classicalArray, 3));

    std::string string("something");
    auto toMatioString = matioCpp::make_variable("string", string);

    std::vector<bool> vecOfBool = {true, false, true};
    auto toVecofBool = matioCpp::make_variable("vecOfBool", vecOfBool);

    auto matioDouble = matioCpp::make_variable("double", 5.0);

    auto matioBool = matioCpp::make_variable("bool", true);

    auto matioInt = matioCpp::make_variable("int", 2);

    auto matioChar = matioCpp::make_variable("char", 'f');

    std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie", ""};
    auto matioCell = matioCpp::make_variable("stringVector", stringVector);

    //It is possible to automatically convert a struct to a matioCpp::Struct
    testStruct s;
    matioCpp::Struct automaticStruct = matioCpp::make_variable("testStruct", s);

    {
        //Saving to file
        matioCpp::File file = matioCpp::File::Create("test.mat"); //Create a new file called "test.mat"

        //Saving to file
        file.write(i);
        file.write(testString);
        file.write(vector);
        file.write(cell_array);
        file.write(struct_test);
        file.write(struct_array);
        file.write(mdarray);
        file.write(toMatioMatrix);
        file.write(toMatioEigenVec);
        file.write(toMatioArray);
        file.write(toMatioClassic);
        file.write(toMatioString);
        file.write(toVecofBool);
        file.write(matioDouble);
        file.write(matioBool);
        file.write(matioInt);
        file.write(matioChar);
        file.write(matioCell);
        file.write(automaticStruct);
        //File is automatically closed at destruction. Alternatively, use file.close();.
    }

    //It is possible to read mat files too.

    matioCpp::File input("test.mat");

    //Read again the data that have been saved.
    auto matioCppVector = input.read("test_vector").asVector<double>();
    Eigen::VectorXd outEigenVector = matioCpp::to_eigen(matioCppVector);

    auto matioCppMatrix = input.read("eigenMatrix").asMultiDimensionalArray<float>();
    Eigen::MatrixXf outMatrix = matioCpp::to_eigen(matioCppMatrix);

    int element = input.read("hello_element").asElement<int>();

    std::string read_string = input.read("string_name").asString()();

    matioCpp::Struct outStruct = input.read("struct").asStruct();
    matioCpp::StructArray outStructArray = input.read("struct_array").asStructArray();

    matioCpp::CellArray outCellArray = input.read("test_cell").asCellArray();

    return EXIT_SUCCESS;
}
