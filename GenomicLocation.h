#ifndef GENOMIC_LOCATION_H
#define GENOMIC_LOCATION_H

/**
 * @file GenomicLocation.h
 * Interface description of the helper class GenomicLocation
 *
 * @author Yi Qiao
 */

namespace SubcloneExplorer {

	/**
	 * @brief A point on a reference genome
	 *
	 * This class represents a point location on a (implied) reference genome.
	 * It consists of a chromosome id, and a position.
	 *
	 * The position is 0 based
	 */
	class GenomicLocation {
		public:
			int chrom; /**< The integer id of the chromosome */
			unsigned long position; /**< The 0-based position */

			/**
			 * minimal constructor to reset all member variables
			 */
			GenomicLocation() : chrom(0), position(0) {}

			/**
			 * GenomicLocation compare operator <
			 *
			 * @param another The other GenomicLocation to compare to
			 * @return true if the object takes place before the other object, false if not
			 */
			inline virtual bool operator<(const GenomicLocation& another) const {
				if(chrom < another.chrom) return true;
				if(chrom > another.chrom) return false;
				if(position < another.position) return true;
				return false;
			}
			
			/**
			 * GenomicLocation compare operator >
			 *
			 * @param another The other GenomicLocation to compare to
			 * @return true if the object takes place after the other object, false if not
			 */
			inline virtual bool operator>(const GenomicLocation& another) const {
				if(chrom > another.chrom) return true;
				if(chrom < another.chrom) return false;
				if(position > another.position) return true;
				return false;
			}
	};
}
#endif
