
struct EdgeCollapseTarget {
    EdgeIter ei;
    double cost;
    double optimalCoord[3];
    int id;

    EdgeCollapseTarget(){}
    EdgeCollapseTarget(EdgeIter &ei_in, double cost_in, double *optimalCoord_in, int id_in) {
        ei   = ei_in;
        cost = cost_in;
        for(int i = 0; i < 3; i++) optimalCoord[i] = optimalCoord_in[i];
        id   = id_in;
    }
};

struct VertexSplitTarget {

    EdgeIter ei;
    double   v1OrginalCoord[3];
    bool     v1OriginalIsBoundary;
    vector<HalfEdge*> halfedgesAroundV0;

    int id;

    VertexSplitTarget(){}
};


class Simplification {
public:
	Simplification() { ect_id_base = 0; }

	void InitSimplification(Mesh *mesh_in);
	bool EdgeCollapse();
	void VertexSplit();
	void ControlLevelOfDetail(int step);
	struct greater {
		bool operator() (EdgeCollapseTarget X, EdgeCollapseTarget Y) {
			return X.cost > Y.cost;
		}
	};
private:
    Mesh *mesh;

    priority_queue <EdgeCollapseTarget, deque<EdgeCollapseTarget>, greater> heap;
    list<EdgeCollapseTarget> suspendedEdgeCollapseTarget;

    stack<VertexSplitTarget>  vertexSplitTarget;
    stack<EdgeCollapseTarget> readdedEdgeCollapseTarget;

    int ect_id_base;  // edge collapse操作的id
    int n_active_faces;

    void AssignInitialQ();  // 为每一个定点分配初始的Q
    void CumulateQ(VertexIter &vi, double *normal, double d);
    void ComputeOptimalCoordAndCost(EdgeIter &ei);
    
    HalfEdge* FindBoundaryEdgeIncidentToVertexInCW(HalfEdge *baseHalfEdge);
    void FindNeighborHalfEdge(VertexIter &v1, vector<FaceIter> &facesOriginallyIncidentToV0OrV1);

    bool IsFinWillNotBeCreated(EdgeIter &ei);
    void RemoveEdge(EdgeIter &ei, double *optimalCoord, bool isFirstCollapse);


};
