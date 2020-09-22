/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <catch2/catch.hpp>
#include <vector>
#include <matioCpp/matioCpp.h>

TEST_CASE("Constructors")
{
    SECTION("Default")
    {
        matioCpp::SharedMatvar shared;
        matioCpp::WeakMatvar weak;
    }

    SECTION("From other")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar shared(matioVar); //shared gets the control of matioVar
        matioCpp::WeakMatvar weak(shared);

        REQUIRE(shared.get() == matioVar);
        REQUIRE(weak.get() == matioVar);
    }

    SECTION("From other non owning")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar* sharedPtr = new matioCpp::SharedMatvar;
        matioCpp::WeakMatvar weak(matioVar, *sharedPtr); //shared does not own matioVar, but weak checks if shared is alive before accessing matioVar

        REQUIRE(weak.get() == matioVar);

        delete sharedPtr;

        REQUIRE(weak.get() == nullptr);

        Mat_VarFree(matioVar);
    }

    SECTION("Copy")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar shared(matioVar); //shared gets the control of matioVar
        matioCpp::WeakMatvar weak(shared);
        matioCpp::SharedMatvar otherShared(shared);
        matioCpp::WeakMatvar otherWeak(weak);

        REQUIRE(shared.get() == matioVar);
        REQUIRE(weak.get() == matioVar);
        REQUIRE(otherShared.get() == matioVar);
        REQUIRE(otherWeak.get() == matioVar);
    }

    SECTION("Move")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar shared(matioVar); //shared gets the control of matioVar
        matioCpp::WeakMatvar weak(shared);
        matioCpp::SharedMatvar otherShared(std::move(shared));
        matioCpp::WeakMatvar otherWeak(std::move(weak));

        REQUIRE(otherShared.get() == matioVar);
        REQUIRE(otherWeak.get() == matioVar);
    }
}

TEST_CASE("Ownership")
{
    SECTION("Shared/Weak")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar* sharedPtr = new matioCpp::SharedMatvar(matioVar);
        matioCpp::WeakMatvar weak(*sharedPtr);

        REQUIRE(sharedPtr->get() == matioVar);
        REQUIRE(weak.get() == matioVar);

        delete sharedPtr;

        REQUIRE(weak.get() == nullptr);
    }

    SECTION("Shared/Shared/Weak")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar* sharedPtr = new matioCpp::SharedMatvar(matioVar);
        matioCpp::WeakMatvar weak(*sharedPtr);
        matioCpp::SharedMatvar otherShared(*sharedPtr);

        REQUIRE(sharedPtr->get() == matioVar);
        REQUIRE(weak.get() == matioVar);

        delete sharedPtr;

        REQUIRE(weak.get() == otherShared.get());
    }
}

TEST_CASE("isShared")
{
    matioCpp::SharedMatvar shared;
    matioCpp::WeakMatvar weak;

    REQUIRE(shared.isShared());
    REQUIRE_FALSE(weak.isShared());
}

TEST_CASE("Duplicate")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar shared;
    matioCpp::WeakMatvar weak;

    REQUIRE(shared.duplicateMatvar(matioVar));
    REQUIRE_FALSE(weak.duplicateMatvar(matioVar));

    double* data = (double *)(matioVar->data);
    data[0] = 1.0;

    REQUIRE(shared.get());

    data = (double*) (shared.get()->data);
    data[0] = 2.0;

    REQUIRE((((double *)(matioVar->data))[0]) != (((double *)(shared.get()->data))[0]));

    Mat_VarFree(matioVar);

    REQUIRE(std::strcmp(shared.get()->name, "test") == 0);

}

