//
// Created by Katie Barr (EI) on 04/11/2016.
//


#include <paths/HyperBasevector.h>
#include <kmers/kmatch/KMatch.h>
#include "lmp_mapper.h"
#include <algorithm>

LMPMapper::LMPMapper(vecbvec lmp_reads, HyperBasevector& hbv, vec<int>& inv, KMatch kmatch): lmp_reads(lmp_reads), hbv(hbv), inv(inv), kMatch(kmatch), read_edge_maps(initalise_read_edge_map()) {}

bool compareEdgeKmerPositions(const edgeKmerPosition &ekp1, const edgeKmerPosition &ekp2){
    // sort by edge id first, then offset. so a higher edge id with a lower offset would be the greater one
    if (ekp1.edge_id < ekp2.edge_id){
        return true;
    } else if (ekp1.edge_id == ekp2.edge_id && ekp1.edge_offset < ekp2.edge_offset){
        // we're on the same edge, so the one with the lowest offset is lower
        return true;
    } else {
        return false;
    }

}


/*ReadPath LMPMapper::getReadMathPair(int edge_id);{

}

ReadPath LMPMapper::getReadMathPair(int read_index){
    ReadPath pair = read_index%2 == 0 ? read_paths[read_index + 1] : read_paths[read_index -1 1]
    return pair;
}*/

// stolen from gonza so blame him!!
std::vector<edgeKmerPosition> LMPMapper::readOffsetFilter(std::vector<edgeKmerPosition> data){
    // Count the number of edges_id that map to each kmer in the read, filter out the edges that map to a position that has more than one edge mapped to it
            // input is a vector with all the links for a specific rea
    std::vector<edgeKmerPosition> links;
    std::map<int, unsigned int> pos_count;
    for (auto l: data){
        // Aumentar el contador, valores son inicializados a 0!?
                pos_count[l.read_offset] += 1;
    }
    // Solo seleccionados lo que estan en offsets de la lectura con un solo edge mapeado
    for (auto l: data){
        if (pos_count[l.read_offset] == 1){
                links.push_back(l);
            }
        }
    return links;
}

void LMPMapper::mapReads(){
    kMatch.Hbv2Map(&hbv);
    std::cout << kMatch.edgeMap.size() << std::endl;
    int mapped_to_single_edge = 0;
    int mappted_to_multiple_edge = 0;
    int unmapped_reads = 0;
    int mapped_to_single_edge_rc = 0;
    int mappted_to_multiple_edge_rc = 0;
    //std::vector<edgeKmerPosition> res = kmatch.lookupRead(lmp_data[0].ToString());
    for (int i=0; i < lmp_reads.size(); i++){
        std::vector<edgeKmerPosition> mapped_edges = kMatch.lookupRead(lmp_reads[i].ToString());
        //std::cout << "Mapped read:" << i << " string " << lmp_reads[i].ToString() << " to " << mapped_edges.size() << "edges" << std::endl;
        read_edge_maps.push_back(mapped_edges);
        int distinct_edge_ids = 0;
        if (mapped_edges.size() != 0){
            int distinct_edge_ids = 1;
            int current_edge_id = mapped_edges[0].edge_id;
        for (auto mapping: mapped_edges){
            if (mapping.edge_id != current_edge_id){
                current_edge_id = mapping.edge_id;
                distinct_edge_ids += 1;

        }
        }
            if (distinct_edge_ids == 1){
                mapped_to_single_edge += 1;
            }
            if (distinct_edge_ids > 1){
                mappted_to_multiple_edge += 1;
            }
        } else {
            if (mapped_edges.size() == 0){
            unmapped_reads += 1;
        } }
    }
    std::cout << "Unmapped reads: " << unmapped_reads << std::endl;
    std::cout << "Mapped to single edge: " << mapped_to_single_edge << std::endl;
    std::cout << "Mapped to multiple edges: " << mappted_to_multiple_edge << std::endl;
    std::cout << "TOtal reads: " << lmp_reads.size() << std:: endl;
}

void LMPMapper::LMPReads2MappedPairedEdgePaths(std::vector<LMPPair > & lmp_pairs_for_scaffolding, std::vector<LMPPair > & lmp_pairs_for_insert_size_estimation){
    mapReads();
    readEdgeMap2LMPPairs(lmp_pairs_for_scaffolding,  lmp_pairs_for_insert_size_estimation);
    std::cout << "LMPReads2MappedPairedEdgePaths lmp pairs size: " << lmp_pairs_for_scaffolding.size() << std::endl;

}

void LMPMapper::readEdgeMap2LMPPairs(std::vector<LMPPair > & lmp_pairs_for_scaffolding, std::vector<LMPPair > & lmp_pairs_for_insert_size_estimation){
    std::vector<LMPPair > read_paths;
    int counter = 0;
    int counter_p1 = 0;
    int counter_p2 = 0;
    for (int i=0; i < read_edge_maps.size() - 1; ++i) {
        LMPPair lmp_pair;
        lmp_pair.read_index = i / 2;
        std::vector<edgeKmerPosition> read_mapping_p1 = read_edge_maps[i];
        //read_mapping_p1 = readOffsetFilter(read_mapping_p1);
        int read_len = static_cast<int>(lmp_reads[i].ToString().size());
        // ensure that full edges will be together, with offsets in increasing order, in read_mapping vector
        lmp_pair.p1 = sortMappingsFindFullyMappedEdges(read_mapping_p1, read_len, i);
        i = i + 1;
        std::vector<edgeKmerPosition> read_mapping_p2 = read_edge_maps[i];
        //read_mapping_p2 = readOffsetFilter(read_mapping_p2);
        lmp_pair.p2 = sortMappingsFindFullyMappedEdges(read_mapping_p2, read_len, i);
        if (lmp_pair.p1.size() != 0) {
            if (lmp_pair.p2.size() != 0) {
                counter += 1;
                for (auto edge1: lmp_pair.p1) {
                    if (lmp_pair.p1.size() == 1 && lmp_pair.p2.size() == 1) {
                        if (lmp_pair.p1[0] == inv[lmp_pair.p2[0]]) {
                            // todo: lmp_pair is wrong data type for this, as we need edge offset
                            lmp_pairs_for_insert_size_estimation.push_back(lmp_pair);
                        }

                    } else {
                        read_paths.push_back(
                                lmp_pair);
                    }

                }

            }
            counter_p1 += 1;

            if (lmp_pair.p2.size() != 0) {
                counter_p2 += 1;
            }
        }
    }
    std::cout << "total both pairs mapped: " << counter << " p1 mapped: " << counter_p1 << "p2 mapped " << counter_p2 << std::endl;
    removeUselessLMPMappings(read_paths, lmp_pairs_for_scaffolding);
    std::cout << "readEdgeMap2LMPPairs lmp pairs size: " << lmp_pairs_for_scaffolding.size() << std::endl;
        std::cout << "pairs map to edge and reverse complement and signle edge: "
                  << lmp_pairs_for_insert_size_estimation.size() << std::endl;


}




ReadPath LMPMapper::sortMappingsFindFullyMappedEdges(std::vector<edgeKmerPosition>  read_mapping, int read_length, int i){
    if (read_mapping.size() > 0) {
        std::sort(read_mapping.begin(), read_mapping.end(), compareEdgeKmerPositions);
        //std::cout << "Read mapping edge id:" << read_mapping[0].edge_id << " offset " << read_mapping[0].offset << " size: " << hbv.EdgeObject(read_mapping[0].edge_id).ToString().length() << " to read of length " << read_length
//                  << std::endl;
        return getFullyMappedEdges(read_mapping, read_length, i);
    }
    // prevent bad access when no reads mapped
    ReadPath empty_path;
    return empty_path;
}


// edges are far longer than reads,
// assume perfect mapping to start with
ReadPath LMPMapper::getFullyMappedEdges(std::vector<edgeKmerPosition> read_mapping, int read_length, int i, int k=31){
    bool is_r2 = i % 2 != 0;
    int kmer_errors_allowed = 0;
    if (is_r2){
        // the read 2 error rate at the ends i much higher, so don't consider the last 25 kmers
        kmer_errors_allowed = (read_length/2)-k;
    }
    int current_edge_id = read_mapping[0].edge_id;
    int consecutive_offsets = 0;
    int last_offset = read_mapping[0].edge_offset;
    ReadPath paths;
    for (auto it = read_mapping.begin(); it != read_mapping.end(); ++it) {
        // if we're still on the same edge
        if (it->edge_id == current_edge_id) {
            // if the edge offset is consecutive
            if (it->edge_offset == (last_offset + 1)) {
                consecutive_offsets += 1;
                /*if (is_r2 && consecutive_offsets > 60) {
                    std::cout << "edge offset: " << last_offset << " consecutiv offsets: " << consecutive_offsets << " kmer errors allowed " << kmer_errors_allowed << " threshold:  " << read_length - k - kmer_errors_allowed << std::endl;
                    std::cout << "Mapping edge: " << hbv.EdgeObject(current_edge_id).ToString().substr(last_offset, 150) << std::endl;
                    std::cout << "to read length" << read_length << " offset " << last_offset << " string " << lmp_reads[i].ToString() << std::endl;
                }*/
                //std::cout << "incrementing consecutive offsets:" << std::endl;

            }
        }
            // if we're onto a new edge, then check if we completed the previous edge -
        else {
           // std::cout << "current consecutively mapped edges: " << consecutive_offsets << "mapping edge: " << hbv.EdgeObject(it->edge_id).ToString() << std::endl;
            // if the number of consecutive offsets is equal to the number of kmers on the read, -1 because we count transitions its fully mapped, L - k
            if ((consecutive_offsets == hbv.EdgeObject(current_edge_id).size() - k) || consecutive_offsets == read_length - k || (is_r2 && (consecutive_offsets > read_length - k - kmer_errors_allowed))) { // lmp reads are shorter than edges, but check both whether entire read OR entire edge is mapped
                //std::cout << "edge mapped:" << it->edge_id << std::endl;
                paths.push_back(current_edge_id);
            }
            current_edge_id = it->edge_id;
            consecutive_offsets = 0;
        }
        last_offset = it->edge_offset;
        // if the above conditional would miss this because we're at the end of the loop, exit here
        if (std::next(it) == read_mapping.end() && ((consecutive_offsets == hbv.EdgeObject(current_edge_id).size() - k) || consecutive_offsets == read_length - k || (is_r2 && (consecutive_offsets > read_length - k - kmer_errors_allowed)))){
            //std::cout << "edge mapped at end:" << it->edge_id << std::endl;
            paths.push_back(current_edge_id);
        }
    }
    //std::cout << "length of path returned: " << paths.size() << std::endl;
    return paths;
}


