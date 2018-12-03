#include "metisUtils.hpp"

#include <utils/prettyprint.hpp>

std::vector<idx_t> partition(std::shared_ptr<Mesh> mesh, idx_t nparts)
{
    using namespace std;
    vector<idx_t> cellsizes;
    vector<idx_t> celldata;
    cellsizes.reserve(mesh->cellCount() + 1);
    celldata.reserve(mesh->cellCount() * 5); //TODO: Arbitrary parameter! Tune this!
    cellsizes.push_back(0);
    for (const auto& cell : mesh->allCells()){
        auto sum = cellsizes[cellsizes.size() - 1];
        cellsizes.push_back(sum + cell.size());
        for (int i = 0; i < cell.size(); i++){
            celldata.push_back(cell.vertex(i));
        }
    }
    idx_t el_count = mesh->cellCount();
    idx_t point_count = mesh->pointCount();
    idx_t objval;
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);
    options[METIS_OPTION_PTYPE] = METIS_PTYPE_RB;
    options[METIS_OPTION_CONTIG] = 1;
    /*options[METIS_OPTION_DBGLVL] = 
        METIS_DBG_INFO |
        METIS_DBG_COARSEN |
        METIS_DBG_REFINE |
        METIS_DBG_IPART ;*/
    options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
    options[METIS_OPTION_NUMBERING] = 0;
    //options[METIS_OPTION_CTYPE] = METIS_CTYPE_RM;
    vector<idx_t> point_partition(mesh->pointCount());
    vector<idx_t> cell_partition(mesh->cellCount());
    int result = METIS_PartMeshNodal(&el_count, &point_count, cellsizes.data(), celldata.data(), 0, 0, &nparts, 0, options, &objval, cell_partition.data(), point_partition.data());
    idx_t *adj;
    idx_t *adjncy;
    idx_t beg = 0;
    METIS_MeshToNodal(&el_count, &point_count, cellsizes.data(), celldata.data(), &beg, &adj, &adjncy);
    return point_partition;
}
