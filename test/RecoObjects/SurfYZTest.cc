#define BOOST_TEST_MODULE ( SurfYZTest )
#include "boost/test/unit_test.hpp"

//
// File: SurfYZTest.cxx
//
// Purpose: Unit test for SurfYZPlane.
//

#include <cmath>
#include "lardata/RecoObjects/SurfYZPlane.h"
#include "lardata/RecoObjects/KalmanLinearAlgebra.h"
#include "cetlib_except/exception.h"

using boost::test_tools::tolerance;
auto const tol = 1.e-6% tolerance();

struct SurfYZTestFixture
{
  SurfYZTestFixture() :
    surf1(),
    surf2(0., 0., 0., 0.),
    surf3(1., 1., 1., 0.),
    surf4(2., 3., 4., 1.) {}
  trkf::SurfYZPlane surf1;  // Default constructed.
  trkf::SurfYZPlane surf2;  // Same as surf1.
  trkf::SurfYZPlane surf3;  // Different origin, parallel to surf1 and surf2.
  trkf::SurfYZPlane surf4;  // Not parallel.
};

BOOST_FIXTURE_TEST_SUITE(SurfYZTest, SurfYZTestFixture)

// Test equality comparisons.

BOOST_AUTO_TEST_CASE(Equality) {
  BOOST_TEST(surf1.isEqual(surf2));
  BOOST_TEST(!surf1.isEqual(surf3));
  BOOST_TEST(!surf1.isEqual(surf4));
  BOOST_TEST(!surf2.isEqual(surf3));
  BOOST_TEST(!surf2.isEqual(surf4));
  BOOST_TEST(!surf3.isEqual(surf4));
}

// Test parallel comparisions.

BOOST_AUTO_TEST_CASE(Parallel) {
  BOOST_TEST(surf1.isParallel(surf2));
  BOOST_TEST(surf1.isParallel(surf3));
  BOOST_TEST(!surf1.isParallel(surf4));
  BOOST_TEST(surf2.isParallel(surf3));
  BOOST_TEST(!surf2.isParallel(surf4));
  BOOST_TEST(!surf3.isParallel(surf4));
}

// Test coordinate transformations.

BOOST_AUTO_TEST_CASE(Transformation) {
  double xyz1[3] = {1., 2., 3.};
  double xyz2[3];
  double uvw[3];
  surf4.toLocal(xyz1, uvw);
  surf4.toGlobal(uvw, xyz2);
  for(int i=0; i<3; ++i)
    BOOST_TEST(xyz1[i] == xyz2[i], tol);
}

// Test separation.

BOOST_AUTO_TEST_CASE(Separation) {
  BOOST_TEST(surf1.distanceTo(surf2) == 0.);
  BOOST_TEST(surf1.distanceTo(surf3) == 1.);
  BOOST_TEST(surf3.distanceTo(surf1) == -1.);
}

// Should throw exception (not parallel).

BOOST_AUTO_TEST_CASE(NotParallel) {
  BOOST_CHECK_EXCEPTION( surf1.distanceTo(surf4), cet::exception,
                         [](cet::exception const & e)
                         {
                           return e.category() == "SurfYZPlane";
                         } );
}

// Test track parameters.

BOOST_AUTO_TEST_CASE(TrackParameters) {
  trkf::TrackVector v(5);
  v(0) = 0.1;   // u.
  v(1) = 0.2;   // v.
  v(2) = 2.;    // du/dw.
  v(3) = 3.;    // dv/dw.
  v(4) = 0.5;   // p = 2 GeV.

  // For this vector, the direction cosines are.
  // du/ds = 2./sqrt(14.);
  // dv/ds = 3./sqrt(14.);
  // dw/ds = 1./sqrt(14.);

  double xyz[3];
  double mom[3];
  surf1.getPosition(v, xyz);
  BOOST_TEST(xyz[0] == 0.1, tol);
  BOOST_TEST(xyz[1] == 0.2, tol);
  BOOST_TEST(xyz[2] == 0.0, tol);
  surf3.getPosition(v, xyz);
  BOOST_TEST(xyz[0] == 1.1, tol);
  BOOST_TEST(xyz[1] == 1.2, tol);
  BOOST_TEST(xyz[2] == 1.0, tol);
  surf1.getMomentum(v, mom, trkf::Surface::FORWARD);
  BOOST_TEST(mom[0] == 4./std::sqrt(14.), tol);
  BOOST_TEST(mom[1] == 6./std::sqrt(14.), tol);
  BOOST_TEST(mom[2] == 2./std::sqrt(14.), tol);
  surf1.getMomentum(v, mom, trkf::Surface::BACKWARD);
  BOOST_TEST(mom[0] == -4./std::sqrt(14.), tol);
  BOOST_TEST(mom[1] == -6./std::sqrt(14.), tol);
  BOOST_TEST(mom[2] == -2./std::sqrt(14.), tol);
  surf4.getMomentum(v, mom, trkf::Surface::FORWARD);
  BOOST_TEST(mom[0] == 4./std::sqrt(14.), tol);
  BOOST_TEST(mom[1] == (6.*std::cos(1.) - 2.*std::sin(1.))/std::sqrt(14.), tol);
  BOOST_TEST(mom[2] == (6.*std::sin(1.) + 2.*std::cos(1.))/std::sqrt(14.), tol);

  // Should throw exception (no direction).

  BOOST_CHECK_EXCEPTION( surf1.getMomentum(v, mom), cet::exception,
                         [](cet::exception const & e)
                         {
                           return e.category() == "SurfYZPlane";
                         } );
}

BOOST_AUTO_TEST_SUITE_END()
