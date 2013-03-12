#ifndef EVENT_CLUSTER_H
#define EVENT_CLUSTER_H

/**
 * @file EventCluster.h
 * Interface description of class EventCluster
 *
 * @author Yi Qiao
 */

#include "Archivable.h"
#include <vector>

namespace SubcloneExplorer {

	// Forward declaration of SomaticEvent so that pointers can be made
	class SomaticEvent;

	/**
	 * @brief A collection class that each instance groups many SomaticEvents which share the same cell frequency
	 *
	 * Clustering is a process to group somatic events with (likely) the same into a single entity, based
	 * on the simplifying (but likely to be true) assumption that events with the same cell frequency are
	 * likely to co-exist in the same subclone
	 *
	 * @see SomaticEvent
	 */
	class EventCluster : public Archivable {
		protected:
			std::vector<SomaticEvent *> _members; /**< the vector that holds all the cluster's members */
			double _cellFraction; /**< the cell fraction all members share */
			
			sqlite3_int64 ofSubcloneID; /**< to which subclone does this cluster belongs */

		protected:
			// Implements Archivable
			virtual std::string getTableName();
			virtual std::string createTableStatementStr();
			virtual std::string createObjectStatementStr();
			virtual std::string updateObjectStatementStr();
			virtual std::string selectObjectColumnListStr();

			virtual int bindObjectToStatement(sqlite3_stmt* statement);
			virtual void updateObjectFromStatement(sqlite3_stmt* statement);

		public:
			/**
			 * Minimal constructor that resets all member variables
			 */
			EventCluster() : Archivable(), _cellFraction(0), ofSubcloneID(0) {;}

			/**
			 * Retrieve the member vector reference
			 *
			 * @return a reference to the members vector
			 */
			inline std::vector<SomaticEvent *> members() const {return _members;}

			/**
			 * Retrieve the cell fraction
			 *
			 * @return cell fraction centroid calculated from all members
			 */
			inline double cellFraction() const {return _cellFraction;}

			/**
			 * Set a new cell fraction
			 *
			 * @param fraction the new cell fraction
			 */
			inline void setCellFraction(double fraction) {_cellFraction = fraction;}

			/**
			 * Add an SomaticEvent object into the member list and update cell fraction
			 *
			 * @param event The event to be added as a member
			 * @param updateFraction Should the method automatically update the cell fraction of the cluster
			 */
			void addEvent(SomaticEvent * event, bool updateFraction = true);

			// Removing an event is such a rare case that, although trivial, it's not yet implemented
			

			/** 
			 * Override the < operator for sorting purpose
			 *
			 * @param another the other EventCluster to compare to
			 * @return true if this object has a smaller cell frequency than the other object, false if otherwise
			 */
			inline bool operator<(const EventCluster &another) const { return _cellFraction < another.cellFraction(); }

			/**
			 * Override the > operator for sorting purpose
			 *
			 * @param another the other EventCluster to compare to
			 * @return true if this object has a greater cell frequency than the other object, false if otherwise
			 */
			inline bool operator>(const EventCluster &another) const { return _cellFraction > another.cellFraction(); }

			/**
			 * Set the subclone ID this cluster belongs to
			 *
			 * @param cloneID the database id of the subclone this cluster belongs to
			 */
			inline void setSubcloneID(sqlite3_int64 cloneID) { ofSubcloneID = cloneID; }

			/**
			 * Retrieve the subclone ID
			 *
			 * @return the subclone id this cluster belongs to
			 */
			inline sqlite3_int64 subcloneID() { return ofSubcloneID; }

			/**
			 * Retrieve a vector of cluster IDs whose parent subclone is the given id
			 *
			 * @param database A live database connection
			 * @param subcloneID The subclone id who contains the clusters
			 *
			 * @return a vector of cluster ids contained by the given subclone id
			 */
			DBObjectID_vec allObjectsOfSubclone(sqlite3 *database, sqlite3_int64 subcloneID);
	};
}
#endif
