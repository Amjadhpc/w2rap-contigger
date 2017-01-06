//
// Created by Katie Barr (EI) on 15/12/2016.
//

#ifndef W2RAP_CONTIGGER_COMPLEXREGION_H
#define W2RAP_CONTIGGER_COMPLEXREGION_H

#include <cstdint>
#include <utility>
#include <vector>
#include <map>
#include <Vec.h>
#include "paths/long/ReadPath.h"

typedef struct {
    uint64_t edge_id;
    std::vector<int> pair_ids;
    std::vector<uint64_t> path_from_center;
    bool into; // true if this is an edge going into the region
    bool forward;
    // as rc edge is always created straigt  after original edge, all originals are even, and rcs are odd- but again, not sure i can depend on this, things get shuffled
} BoundingEdge;

/*
 * when a region which is solveable is found, create an object for it to hold the paths
 * then, consistency between regions can be validated before tying to separate,
 * and paths which resolve a region can be selected over ones that don't
 */

    class ComplexRegion {
    public:
        ComplexRegion::ComplexRegion(std::vector<uint64_t> edges_in, std::vector<uint64_t> edges_out,
                                     vec<int> &involution, int insert_size = 5000);

        ComplexRegion::ComplexRegion();

        std::vector<uint64_t> edges_in;
        std::vector<uint64_t> edges_out;
        std::vector<std::vector<uint64_t> > paths;
        vec<int> involution;// would be nicer to have the region 'know' its in a collection, and use involution from there

        std::vector < std::vector<uint64_t> >  BuildPaths();

        bool solved;
        int insert_size;
        // to ensure all paths are on same part of the graph, in same direction
        void AddPairId(int edge_index, int pair_id, bool in);
        void AddPathTo(int edge_index, bool in, std::vector<uint64_t> path_from_center);
        void FindSolveablePairs();
        void isSolved(int min_count);
        std::vector<std::pair<uint64_t, uint64_t> > combinations_to_use;


    private:
        std::vector<uint64_t>  BuildPath(BoundingEdge edge_in, BoundingEdge edge_out);
        std::map<std::pair<uint64_t, uint64_t>, int > combination_counts;
        std::vector<uint64_t > in_edges_solved;
        std::vector<uint64_t > out_edges_solved;

        std::vector<BoundingEdge> edges_in_detailed;
        std::vector<BoundingEdge> edges_out_detailed;
        std::string path_str(std::vector<uint64_t> path);

    };


class ComplexRegionCollection {
public:
    ComplexRegionCollection(vec<int> &involution);
    bool AddRegion(std::vector<uint64_t> edges_in, std::vector<uint64_t> edges_out,
                   vec<int> &involution, int insert_size = 5000);
    bool ContainsRegionWithEdges(std::vector<uint64_t> edges_in, std::vector<uint64_t> edges_out);
    std::pair<ComplexRegion, int> GetRegionWithEdges(std::vector<uint64_t> edges_in, std::vector<uint64_t> edges_out);
    void SelectRegionsForPathSeparation();
    std::vector<std::vector<uint64_t> > GetPathsToSeparate();
    std::vector<ComplexRegion> complex_regions;

private:
    ComplexRegion FindBestSolvedRegion(ComplexRegion region_1, ComplexRegion region_2);
    std::string path_str(std::vector<uint64_t> path);
    std::vector<ComplexRegion> solved_regions;
    std::vector<ComplexRegion> solved_regions_final;
    std::map<std::pair< std::vector<uint64_t>, std::vector<uint64_t> >, int> edges_to_region_index;
    vec<int> involution;

};



#endif //W2RAP_CONTIGGER_COMPLEXREGION_H