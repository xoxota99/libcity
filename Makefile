COMPILER=g++
COMPILER_FLAGS=-Wall -fPIC -pedantic -g

ARCHIVER=ar
ARCHIVER_FLAGS=rcs

LIB_FILENAME=libcity
TEST_FILENAME=unit_tests

HEADERS_DIR=include/
DOCUMENTATION_DIR=doc

UNITTESTCPP_LIB=../UnitTest++/libUnitTest++.a
UNITTESTCPP_INCLUDE_DIR=../UnitTest++/src/

HEADERS_INSTALL_PATH=/usr/local/include
LIB_INSTALL_PATH=/usr/local/lib

.PHONY: install uninstall static dynamic clean doc headers test

all: static dynamic headers

# LIB Object files and sources ##########################

# Geometry package
GEOMETRY_PACKAGE=src/geometry/line.o \
                 src/geometry/linesegment.o \
                 src/geometry/point.o \
                 src/geometry/vector.o \
                 src/geometry/polygon.o \
                 src/geometry/ray.o \
                 src/geometry/shape.o

# Streetgraph package
STREETGRAPH_PACKAGE=src/streetgraph/intersection.o \
                    src/streetgraph/road.o \
                    src/streetgraph/streetgraph.o \
                    src/streetgraph/path.o \
                    src/streetgraph/rasterroadpattern.o \
                    src/streetgraph/organicroadpattern.o \
                    src/streetgraph/areaextractor.o

# LSystem package
LSYSTEM_PACKAGE=src/lsystem/lsystem.o \
                src/lsystem/graphiclsystem.o \
                src/lsystem/roadlsystem.o

# Regions package
REGIONS_PACKAGE=src/area/block.o \
                src/area/area.o \
                src/area/zone.o \
                src/area/lot.o \
                src/area/subregion.o

# Entities package
ENTITIES_PACKAGE=src/entities/urbanentity.o \
                 src/entities/building.o

# No package
MISC=src/random.o \
     src/city.o

LIB_OBJECTS=$(GEOMETRY_PACKAGE) $(STREETGRAPH_PACKAGE) $(LSYSTEM_PACKAGE) $(REGIONS_PACKAGE) $(ENTITIES_PACKAGE) $(MISC)

$(LIB_OBJECTS): %.o: %.cpp %.h
	$(COMPILER) $(COMPILER_FLAGS) -c $< -o $@



# TEST Object files and sources ##########################

# Unit tests
TEST_UNITS=test/testPoint.o   \
           test/testVector.o  \
           test/testPolygon.o \
           test/testLSystem.o \
           test/testGraphicLSystem.o \
           test/testStreetGraph.o \
           test/testLine.o \
           test/testLineSegment.o \
           test/testRasterRoadPattern.o \
           test/testRandom.o \
           test/testAreaExtractor.o \
           test/testPath.o \
           test/testRay.o \
           test/testBlock.o \
           test/testLot.o \
           test/testZone.o \
           test/testSubRegion.o \
           test/testShape.o

TEST_MAIN=test/main.o
TEST_OBJECTS=$(TEST_UNITS) $(TEST_MAIN)

$(TEST_OBJECTS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -I$(UNITTESTCPP_INCLUDE_DIR) -c $< -o $@

static: $(LIB_OBJECTS)
	$(ARCHIVER) $(ARCHIVER_FLAGS) $(LIB_FILENAME).a $(LIB_OBJECTS)

dynamic: $(LIB_OBJECTS)
	$(COMPILER) -shared -o $(LIB_FILENAME).so $(LIB_OBJECTS)

headers:
	mkdir -p $(HEADERS_DIR)
	cd src/ && find . -name "*.h" -exec rsync -R {} ../$(HEADERS_DIR) \; >/dev/null

install: all
	mkdir -p $(HEADERS_INSTALL_PATH)/libcity
	cp -r include/* $(HEADERS_INSTALL_PATH)/libcity
	cp libcity.h $(HEADERS_INSTALL_PATH)
	cp libcity.so $(LIB_INSTALL_PATH)
	cp libcity.a $(LIB_INSTALL_PATH)

uninstall:
	rm -rf $(HEADERS_INSTALL_PATH)/libcity
	rm -f $(HEADERS_INSTALL_PATH)/libcity.h
	rm -f $(LIB_INSTALL_PATH)/libcity.so
	rm -f $(LIB_INSTALL_PATH)/libcity.a

test: $(TEST_OBJECTS) static
	$(COMPILER) $(COMPILER_FLAGS) -I$(UNITTESTCPP_INCLUDE_DIR) -o $(TEST_FILENAME) $(TEST_OBJECTS) $(UNITTESTCPP_LIB) libcity.a

doc:
	rm -rf doc/
	doxygen Doxyfile

clean:
	rm -rf *.o *.so *.a *~
	rm -rf $(DOCUMENTATION_DIR)
	rm -rf $(HEADERS_DIR)
	rm -f $(LIB_OBJECTS)
	rm -f $(TEST_OBJECTS)
