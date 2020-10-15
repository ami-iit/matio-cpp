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
        matioCpp::MatvarHandler* upcasted = sharedPtr;
        matioCpp::WeakMatvar weak(matioVar, *sharedPtr); //shared does not own matioVar, but weak checks if shared is alive before accessing matioVar

        matioCpp::WeakMatvar weakFromUpcasted(matioVar, upcasted);

        REQUIRE(weak.get() == matioVar);
        REQUIRE(weakFromUpcasted.get() == matioVar);

        delete sharedPtr;

        REQUIRE(weak.get() == nullptr);
        REQUIRE(weakFromUpcasted.get() == nullptr);

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

TEST_CASE("Import")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar shared;
    matioCpp::WeakMatvar weak;

    REQUIRE(shared.importMatvar(matioVar));
    REQUIRE_FALSE(weak.importMatvar(matioVar));
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

TEST_CASE("Pointer to duplicate")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar* sharedPtr = new matioCpp::SharedMatvar(matioVar);

    matioCpp::MatvarHandler* duplicate = sharedPtr->pointerToDuplicate();

    REQUIRE(duplicate->get() == sharedPtr->get());
    REQUIRE(duplicate->isShared());

    delete sharedPtr;
    REQUIRE(duplicate->get());

    delete duplicate;

}

TEST_CASE("Weak ownership")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar* sharedPtr = new matioCpp::SharedMatvar(matioVar);
    matioCpp::MatvarHandler* upcasted = sharedPtr;
    matioCpp::WeakMatvar* weak = new matioCpp::WeakMatvar(*sharedPtr);

    matioCpp::WeakMatvar weakFromShared = upcasted->weakOwnership();
    upcasted = weak;
    matioCpp::WeakMatvar weakFromWeak = upcasted->weakOwnership();

    REQUIRE(weakFromShared.get() == sharedPtr->get());
    REQUIRE_FALSE(weakFromShared.isShared());

    REQUIRE(weakFromWeak.get() == weak->get());
    REQUIRE_FALSE(weakFromWeak.isShared());

    delete sharedPtr;
    delete weak;
    REQUIRE_FALSE(weakFromShared.get());
    REQUIRE_FALSE(weakFromWeak.get());
}

TEST_CASE("Weak ownership register")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    matvar_t* importedVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar shared;
    matioCpp::WeakMatvar weak(matioVar, shared); //shared does not own matioVar, but weak checks if shared is alive before accessing matioVar

    REQUIRE(weak.get() == matioVar);

    shared.dropOwnedPointer(matioVar); //This is the case when setting a cell for example.

    REQUIRE_FALSE(weak.get());

    matioCpp::WeakMatvar otherWeak(matioVar, shared); //shared does not own matioVar, but weak checks if shared is alive before accessing matioVar
    REQUIRE(otherWeak.get() == matioVar);
    shared.importMatvar(importedVar);
    REQUIRE_FALSE(otherWeak.get());

    Mat_VarFree(matioVar);
}

TEST_CASE("Weak ownership register with deletion")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    matvar_t* importedVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar shared;
    matioCpp::WeakMatvar weak(matioVar, &shared, matioCpp::DeleteMode::Delete);

    REQUIRE(weak.get() == matioVar);

    shared.dropOwnedPointer(matioVar); //This is the case when setting a cell for example.

    REQUIRE_FALSE(weak.get());

    matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);

    matioCpp::WeakMatvar otherWeak(matioVar, shared, matioCpp::DeleteMode::Delete);
    REQUIRE(otherWeak.get() == matioVar);
    shared.importMatvar(importedVar);
    REQUIRE_FALSE(otherWeak.get());
}
