#include "ExtraObjectBlock.h"

using namespace LibOpenNFS::NFS2;

template class LibOpenNFS::NFS2::ExtraObjectBlock<PS1>;
template class LibOpenNFS::NFS2::ExtraObjectBlock<PC>;

template <typename Platform>
ExtraObjectBlock<Platform>::ExtraObjectBlock(std::ifstream &trk)
{
    ASSERT(this->_SerializeIn(trk), "Failed to serialize ExtraObjectBlock from file stream");
}

template <typename Platform>
bool ExtraObjectBlock<Platform>::_SerializeIn(std::ifstream &ifstream)
{
    // Read the header
    SAFE_READ(ifstream, &recSize, sizeof(uint32_t));
    SAFE_READ(ifstream, &id, sizeof(uint16_t));
    SAFE_READ(ifstream, &nRecords, sizeof(uint16_t));

    switch (id)
    {
    case 2: // First xblock always texture table (in COL)
        nTextures = nRecords;
        polyToQfsTexTable.reserve(nTextures);
        SAFE_READ(ifstream, polyToQfsTexTable.data(), nTextures * sizeof(TEXTURE_BLOCK));
        break;
    case 4:
        nNeighbours = nRecords;
        blockNeighbours.reserve(nRecords);
        SAFE_READ(ifstream, blockNeighbours.data(), nRecords * sizeof(uint16_t));
        break;
    case 5:
        polyTypes.reserve(nRecords);
        SAFE_READ(ifstream, polyTypes.data(), nRecords * sizeof(POLY_TYPE));
        break;
    case 6:
        medianData.reserve(nRecords);
        SAFE_READ(ifstream, medianData.data(), nRecords * sizeof(MEDIAN_BLOCK));
        break;
    case 7:
    case 18:
    case 19:
        nStructureReferences += nRecords;
        for (uint32_t structureRef_Idx = 0; structureRef_Idx < nStructureReferences; ++structureRef_Idx)
        {
            structureBlocks.push_back(StructureBlock(ifstream));
        }
        break;
    case 8: // XBID 8 3D Structure data: This block is only present if nExtraBlocks != 2 (COL)
        structures.reserve(nRecords);
        nStructures = nRecords;
        for (uint32_t structureIdx = 0; structureIdx < nStructures; ++structureIdx)
        {
            structures.push_back(GeomBlock<Platform>(ifstream));
        }
        break;
    case 9:
        nLanes = nRecords;
        laneData.reserve(nRecords);
        SAFE_READ(ifstream, laneData.data(), nLanes * sizeof(LANE_BLOCK));
        break;
    case 10: // PS1 Specific id, Misc purpose
    {
        std::cout << "id 10 NStruct: " << nRecords << std::endl;
        // PS1::TRKBLOCK *ps1TrackBlock = ((PS1::TRKBLOCK *) trackblock);
        // ps1TrackBlock->nUnknownVerts = nRecords;
        // uint8_t xbidHeader[8];
        // trk.read((char *) xbidHeader, 8);
        // for (int i = 0; i < 8; ++i)
        // {
        //     std::cout << (int) xbidHeader[i] << std::endl;
        // }
        // // TODO: Likely these are not VERTS, and the act of adding the parent block center gives meaning where none is
        // present.ps1TrackBlock->unknownVerts = new PS1::VERT[nRecords];
        // for (uint32_t record_Idx = 0; record_Idx < nRecords; ++record_Idx)
        // {
        //     trk.read((char *) &ps1TrackBlock->unknownVerts[record_Idx], sizeof(PS1::VERT));
        // }
    }
    break;
    case 13:
        nVroad = nRecords;
        vroadData.reserve(nRecords);
        SAFE_READ(ifstream, vroadData.data(), nVroad * sizeof(typename Platform::VROAD));
        break;
    case 15:
        nCollisionData = nRecords;
        collisionData.reserve(nCollisionData);
        SAFE_READ(ifstream, collisionData.data(), nCollisionData * sizeof(COLLISION_BLOCK));
        break;
    default:
        LOG(WARNING) << "Unknown id: " << id << " nRecords: " << nRecords << " RecSize: " << recSize;
        break;
    }

    LOG(DEBUG) << "XBID: " << id;

    return true;
}

template <typename Platform>
void ExtraObjectBlock<Platform>::_SerializeOut(std::ofstream &ofstream)
{
    ASSERT(false, "ExtraObjectBlock output serialization is not currently implemented");
}
