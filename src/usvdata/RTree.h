#ifndef USV_GUI_RTREE_H
#define USV_GUI_RTREE_H


#include <vector>
#include <stdexcept>
#include <cmath>

namespace Hilbert {
    uint32_t deinterleave(uint32_t x) {
        x = x & 0x55555555;
        x = (x | (x >> 1)) & 0x33333333;
        x = (x | (x >> 2)) & 0x0F0F0F0F;
        x = (x | (x >> 4)) & 0x00FF00FF;
        x = (x | (x >> 8)) & 0x0000FFFF;
        return x;
    }

    uint32_t interleave(uint32_t x) {
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;
        return x;
    }

    uint32_t prefixScan(uint32_t x) {
        x = (x >> 8) ^ x;
        x = (x >> 4) ^ x;
        x = (x >> 2) ^ x;
        x = (x >> 1) ^ x;
        return x;
    }

    uint32_t descan(uint32_t x) {
        return x ^ (x >> 1);
    }

    void hilbertIndexToXY(uint32_t n, uint32_t i, uint32_t& x, uint32_t& y) {
        i = i << (32 - 2 * n);

        uint32_t i0 = deinterleave(i);
        uint32_t i1 = deinterleave(i >> 1);

        uint32_t t0 = (i0 | i1) ^0xFFFF;
        uint32_t t1 = i0 & i1;

        uint32_t prefixT0 = prefixScan(t0);
        uint32_t prefixT1 = prefixScan(t1);

        uint32_t a = (((i0 ^ 0xFFFF) & prefixT1) | (i0 & prefixT0));

        x = (a ^ i1) >> (16 - n);
        y = (a ^ i0 ^ i1) >> (16 - n);
    }

    uint32_t hilbertXYToIndex(uint32_t n, uint32_t x, uint32_t y) {
        x = x << (16 - n);
        y = y << (16 - n);

        uint32_t A, B, C, D;

        // Initial prefix scan round, prime with x and y
        {
            uint32_t a = x ^y;
            uint32_t b = 0xFFFF ^a;
            uint32_t c = 0xFFFF ^(x | y);
            uint32_t d = x & (y ^ 0xFFFF);

            A = a | (b >> 1);
            B = (a >> 1) ^ a;

            C = ((c >> 1) ^ (b & (d >> 1))) ^ c;
            D = ((a & (c >> 1)) ^ (d >> 1)) ^ d;
        }

        {
            uint32_t a = A;
            uint32_t b = B;
            uint32_t c = C;
            uint32_t d = D;

            A = ((a & (a >> 2)) ^ (b & (b >> 2)));
            B = ((a & (b >> 2)) ^ (b & ((a ^ b) >> 2)));

            C ^= ((a & (c >> 2)) ^ (b & (d >> 2)));
            D ^= ((b & (c >> 2)) ^ ((a ^ b) & (d >> 2)));
        }

        {
            uint32_t a = A;
            uint32_t b = B;
            uint32_t c = C;
            uint32_t d = D;

            A = ((a & (a >> 4)) ^ (b & (b >> 4)));
            B = ((a & (b >> 4)) ^ (b & ((a ^ b) >> 4)));

            C ^= ((a & (c >> 4)) ^ (b & (d >> 4)));
            D ^= ((b & (c >> 4)) ^ ((a ^ b) & (d >> 4)));
        }

        // Final round and projection
        {
            uint32_t a = A;
            uint32_t b = B;
            uint32_t c = C;
            uint32_t d = D;

            C ^= ((a & (c >> 8)) ^ (b & (d >> 8)));
            D ^= ((b & (c >> 8)) ^ ((a ^ b) & (d >> 8)));
        }

        // Undo transformation prefix scan
        uint32_t a = C ^(C >> 1);
        uint32_t b = D ^(D >> 1);

        // Recover index bits
        uint32_t i0 = x ^y;
        uint32_t i1 = b | (0xFFFF ^ (i0 | a));

        return ((interleave(i1) << 1) | interleave(i0)) >> (32 - 2 * n);
    }

// These are multiplication tables of the alternating group A4,
// preconvolved with the mapping between Morton and Hilbert curves.
    static const uint8_t mortonToHilbertTable[] = {
            48, 33, 27, 34, 47, 78, 28, 77,
            66, 29, 51, 52, 65, 30, 72, 63,
            76, 95, 75, 24, 53, 54, 82, 81,
            18, 3, 17, 80, 61, 4, 62, 15,
            0, 59, 71, 60, 49, 50, 86, 85,
            84, 83, 5, 90, 79, 56, 6, 89,
            32, 23, 1, 94, 11, 12, 2, 93,
            42, 41, 13, 14, 35, 88, 36, 31,
            92, 37, 87, 38, 91, 74, 8, 73,
            46, 45, 9, 10, 7, 20, 64, 19,
            70, 25, 39, 16, 69, 26, 44, 43,
            22, 55, 21, 68, 57, 40, 58, 67,
    };

    static const uint8_t hilbertToMortonTable[] = {
            48, 33, 35, 26, 30, 79, 77, 44,
            78, 68, 64, 50, 51, 25, 29, 63,
            27, 87, 86, 74, 72, 52, 53, 89,
            83, 18, 16, 1, 5, 60, 62, 15,
            0, 52, 53, 57, 59, 87, 86, 66,
            61, 95, 91, 81, 80, 2, 6, 76,
            32, 2, 6, 12, 13, 95, 91, 17,
            93, 41, 40, 36, 38, 10, 11, 31,
            14, 79, 77, 92, 88, 33, 35, 82,
            70, 10, 11, 23, 21, 41, 40, 4,
            19, 25, 29, 47, 46, 68, 64, 34,
            45, 60, 62, 71, 67, 18, 16, 49,
    };

    uint32_t transformCurve(uint32_t in, uint32_t bits, const uint8_t* lookupTable) {
        uint32_t transform = 0;
        uint32_t out = 0;

        for (int32_t i = 3 * (bits - 1); i >= 0; i -= 3) {
            transform = lookupTable[transform | ((in >> i) & 7)];
            out = (out << 3) | (transform & 7);
            transform &= ~7;
        }

        return out;
    }

    uint32_t mortonToHilbert3D(uint32_t mortonIndex, uint32_t bits) {
        return transformCurve(mortonIndex, bits, mortonToHilbertTable);
    }

    uint32_t hilbertToMorton3D(uint32_t hilbertIndex, uint32_t bits) {
        return transformCurve(hilbertIndex, bits, hilbertToMortonTable);
    }
}

template<size_t nodeSize>
class RTree {
    size_t num_items_;
    double minX{std::numeric_limits<double>::infinity()};
    double minY{std::numeric_limits<double>::infinity()};
    double maxX{-std::numeric_limits<double>::infinity()};
    double maxY{-std::numeric_limits<double>::infinity()};
    struct LevelBound {

    };
    struct BBox {
        double minX;
        double minY;
        double maxX;
        double maxY;
    };

    std::vector<LevelBound> level_bounds_;
    std::vector<size_t> indices_;
    std::vector<BBox> bboxes_;
    BBox root_bbox{
            std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()
    };

    explicit RTree(size_t numItems) : num_items_(numItems), level_bounds_({numItems}) {
        if (numItems <= 0) throw std::runtime_error("Zero rtree items size.");

        // calculate the total number of nodes in the R-tree to allocate space for
        // and the index of each tree level (used in search later)
        auto n = numItems;
        auto numNodes = n;
        do {
            n = std::ceil(n / nodeSize);
            numNodes += n;
            level_bounds_.push_back(numNodes * 4);
        } while (n != 1);


        data = new ArrayBuffer(8 + nodesByteSize + numNodes * this.IndexArrayType.BYTES_PER_ELEMENT);
        _boxes = new
        this.ArrayType(this.data, 8, numNodes * 4);
        _indices = new
        this.IndexArrayType(this.data, 8 + nodesByteSize, numNodes);
        _pos = 0;

        new Uint8Array(this.data, 0, 2).set([0xfb, (VERSION << 4) + arrayTypeIndex]);
        new Uint16Array(this.data, 2, 1)[0] = nodeSize;
        new Uint32Array(this.data, 4, 1)[0] = numItems;

        // a priority queue for k-nearest-neighbors queries
        this._queue = new FlatQueue();
    }

    void add(BBox bbox, size_t index) {
        if(num_items_-1==bboxes_.size())
            throw std::runtime_error("Too many items.");

        indices_.push_back(index);
        bboxes_.push_back(bbox);

        if (bbox.minX < root_bbox.minX) root_bbox.minX = minX;
        if (minY < root_bbox.minY) root_bbox.minY = minY;
        if (maxX > root_bbox.maxX) root_bbox.maxX = maxX;
        if (maxY > root_bbox.maxY) root_bbox.maxY = maxY;
    }

    void finish() {
        if (bboxes_.size() != num_items_)
            throw std::runtime_error("Lack items.");

        if (num_items_ <= nodeSize) {
            // only one node, skip sorting and just fill the root box
            bboxes_[this._pos++] = this.minX;
            bboxes_[this._pos++] = this.minY;
            bboxes_[this._pos++] = this.maxX;
            bboxes_[this._pos++] = this.maxY;
            return;
        }

        const auto width = root_bbox.maxX - root_bbox.minX;
        const auto height = root_bbox.maxY - root_bbox.minY;
        std::vector<uint32_t> hilbertValues;
        const auto hilbertMax = (1 << 16) - 1;

        // map item centers into Hilbert coordinate space and calculate Hilbert values
        for (const auto& bbox:bboxes_) {
            const auto x = std::floor(hilbertMax * ((bbox.minX + bbox.maxX) / 2 - this.minX) / width);
            const auto y = std::floor(hilbertMax * ((bbox.minY + bbox.maxY) / 2 - this.minY) / height);
            hilbertValues.push_back(Hilbert::hilbertXYToIndex()hilbert(x, y));
        }

        // sort items by their Hilbert value (for packing later)
        sort(hilbertValues, this._boxes, this._indices, 0, this.numItems - 1, this.nodeSize);

        // generate nodes at each tree level, bottom-up
        for (let i = 0, pos = 0; i < this._levelBounds.length - 1; i++) {
            const end = this._levelBounds[i];

            // generate a parent node for each block of consecutive <nodeSize> nodes
            while (pos < end) {
                const nodeIndex = pos;

                // calculate bbox for the new node
                let nodeMinX = Infinity;
                let nodeMinY = Infinity;
                let nodeMaxX = -Infinity;
                let nodeMaxY = -Infinity;
                for (let i = 0; i < this.nodeSize && pos < end; i++) {
                    nodeMinX = Math.min(nodeMinX, this._boxes[pos++]);
                    nodeMinY = Math.min(nodeMinY, this._boxes[pos++]);
                    nodeMaxX = Math.max(nodeMaxX, this._boxes[pos++]);
                    nodeMaxY = Math.max(nodeMaxY, this._boxes[pos++]);
                }

                // add the new node to the tree data
                this._indices[this._pos >> 2] = nodeIndex;
                this._boxes[this._pos++] = nodeMinX;
                this._boxes[this._pos++] = nodeMinY;
                this._boxes[this._pos++] = nodeMaxX;
                this._boxes[this._pos++] = nodeMaxY;
            }
        }
    }


};

#endif //USV_GUI_RTREE_H
