//The MIT License (MIT)
//Copyright (c) 2016 Massachusetts Institute of Technology

//Authors: Mike Gowanlock
//This software has been created in projects supported by the US National
//Science Foundation and NASA (PI: Pankratius, NSF ACI-1442997, NASA AIST-NNX15AG84G)


//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#include "structs.h"
#include <vector>
#include "RTree.h"



using namespace std;


class DBScan{

	////////////////////////////////////
	//Public
	////////////////////////////////////	
	public:
	

	///Constructor for the implementation that reuses results from one variant to another variant:	
	///Input: 
	///pointer to the data elements (*ptrData)
	///the distance: epsilon
	///the minimum number of points to form a cluster: minimumPts
	///a pointer to the R-tree index: *indexPtr
	///a lookup array to the data elements stored in each MBB of the index: *ptr_MPB_lookup
 	///The high resolution R-tree that's used when "growing the cluster" when reusing data: *highResIndex
	DBScan(std::vector<struct dataElem> *ptrData, double epsilon, int minimumPts, 
		RTree<int,double,2,float> *indexPtr, std::vector<std::vector<int> > *ptr_MPB_lookup, RTree<int,double,2,float> *highResIndex);

	///Allows clustering in parallel of multiple variants
	///Uses a single R-tree index, but separate buffers for each thread
	///
	void algDBScanParallel(); /*!< */

	///DBSCAN version that takes the clustered output from one instance and then reuses it for another instance
	///
	///
	void algDBScanParallelReuseClusterResults(); /*!< */

	///Method that generates an MBB around a cluster for reusing data between variants
	///
	///
	void generateMBBAroundCluster(std::vector<int>*clusterPoints, double * MBB_min, double * MBB_max); 

	
	
	void appendMBBByEpsilon(double * MBB_min, double * MBB_max, double eps); /*!<Method that appends an MBB by epsilon */

	///"Stump" of DBScan used when assigning points to predefined clusters
	///just the part of the algorithm that expands the eps-neighbourhood
	///
	void DBScanParallelMPBSTUMP(int finishedInstanceID, bool * destroyedArr, std::vector<int>* candidatesToGrowFrom); /*!< */

	
	void assignPointsToPredefinedCluster(int finishedInstanceID); /*!< Method that builds new clusters from previously generated clusters*/

	///Schedule of cluster reuse processing based on density (ClusDensity in the paper)
	///
	///
	void setClusterScheduleDensity(std::vector<int>* schedule, std::vector<int> clusterArr[], int numClustersInOtherInstance); /*!< */
	
	///Selects the cluster reuse criteria
	///ClusDensity in the paper only
	///
	void scheduleSelector(std::vector<int>* schedule, std::vector<int> clusterArr[], int numClustersInOtherInstance); /*!< */
	
	static bool comparedensityStructfn(const densityStruct &a, const densityStruct &b); /*!< Comparison function*/

	int getDBScanNumClusters(); /*!<Gets the number of clusters generated by a DBScan instance for reusing data */

	///Vector that keeps track of the assignment of the points to a cluster. Cluster 0 means a noise point.
	///The indices of the vector correspond to the data points.
	///Element i in clusterIDs corresponds to the cluster that data element i is in within the dataPoints struct.
	std::vector<int>clusterIDs;

	std::vector<DBScan *> instanceVect; /*!< Vector of pointers to the DBScan instances that may have points for data reuse*/

	int numClustersForStats; /*!< The number of clusters created (for statistics) */

	////////////////////////////////////
	//Private
	////////////////////////////////////
	private:
	

	

	double distance; /*!< DBSCAN Epsilon parameter */
	int minPts; /*!< DBSCAN MinPts parameter */
	int clusterCnt; /*!<  the number of clusters found after calling the algorithm */

	
	std::vector<bool>visited; /*!< vector that keeps track of the points that have beeen visited */

	std::vector<struct dataElem> *dataPoints; /*!< pointer to the data elements */

	std::vector<std::vector<int> > *ptrMPBlookup; /*!< pointer to the lookup array for multiple pointers per MBB (MPB) */

	
	std::vector<int> setIDsInDist; /*!< temporary vector used to store the ids of the candidates that are actually within the threshold distance */	

	RTree<int,double,2,float> *tree; /*!<  pointer to the R-tree index */ 

	RTree<int,double,2,float> *treeHighResolution; /*!<  pointer to the R-tree index that has 1 point per box when results can be reused across clustering runs */	 

	void initializeVisitedPoints(int size); /*!<  initialize all of the points to initially not be visited */

	void generateMBB(struct dataElem * point, double distance, double * MBB_min, double * MBB_max); /*!< MBB generation. The "Normal" refers to when we had the periodic boundary condition for the datasets and we did not need to wrap around generate a query MBB around the point to search for the values */
	bool generateMBBNormal(struct dataElem * point, double distance, double * MBB_min, double * MBB_max); /*!<  MBB generation. The "Normal" refers to when we had the periodic boundary condition for the datasets and we did not need to wrap around generate a query MBB around the point to search for the values*/

    int filterCandidatesMPB(struct dataElem * point, std::vector<int> * candidateSet, double distance, std::vector<int> * setIDsInDistPtr); /*!< Filter candidate points from the index search */

    void getNeighboursParallelMPB(struct dataElem * point, double distance, std::vector<int> * setIDsInDistPtr); /*!< Epsilon-neighborhood search */

	double EuclidianDistance(struct dataElem * point1, struct dataElem * point2); /*!<  Euclidian distance calculation between two points to filter the candidate set*/

	void copyVect(std::vector<int> * dest, std::vector<int> * source); /*!<  copies the contents from the source vector and appends them to the dest vector */

	void initializeClusterIDs(int size); /*!< initializes the vector storing the IDs of the cluster of the data points */
	

};









