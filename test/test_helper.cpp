#include <scope/test.h>

#include "utility_impl.h"

#include "test_helper.h"

void edge(Graph::vertex source, Graph::vertex target, Graph& fsm, TransitionPtr tPtr) {

  while (source >= fsm.numVertices()) fsm.addVertex();
  while (target >= fsm.numVertices()) fsm.addVertex();

  fsm.addEdge(source, target);
  fsm[target] = tPtr;
}

void edge(Graph::vertex source, Graph::vertex target, Graph& fsm, Transition* tPtr) {
  edge(source, target, fsm, TransitionPtr(tPtr));
}

void ASSERT_SUPERGRAPH(const Graph& a, const Graph& b) {
  for (uint32 av = 0; av < a.numVertices(); ++av) {
    SCOPE_ASSERT(av < b.numVertices());

    for (uint32 a_ov = 0; a_ov < a.outDegree(av); ++a_ov) {
      SCOPE_ASSERT(a.outVertex(av, a_ov) < b.numVertices());
      SCOPE_ASSERT(b.edgeExists(av, a.outVertex(av, a_ov)));
    }
  }
}

void ASSERT_EQUAL_GRAPHS(const Graph& a, const Graph& b) {
  SCOPE_ASSERT_EQUAL(a.numVertices(), b.numVertices());
  ASSERT_SUPERGRAPH(a, b);
  ASSERT_SUPERGRAPH(b, a);
}

void ASSERT_EQUAL_LABELS(const Graph& a, const Graph& b) {
  SCOPE_ASSERT_EQUAL(a.numVertices(), b.numVertices());
  for (uint32 v = 0; v < a.numVertices(); ++v) {
    SCOPE_ASSERT((!a[v] && !b[v]) || (a[v] && b[v]));
    if (a[v] && b[v]) {
      SCOPE_ASSERT_EQUAL(a[v]->Label, b[v]->Label);
    }
  }
}

void ASSERT_EQUAL_MATCHES(const Graph& a, const Graph& b) {
  SCOPE_ASSERT_EQUAL(a.numVertices(), b.numVertices());
  for (uint32 v = 0; v < a.numVertices(); ++v) {
    SCOPE_ASSERT((!a[v] && !b[v]) || (a[v] && b[v]));
    if (a[v] && b[v]) {
      SCOPE_ASSERT_EQUAL(a[v]->IsMatch, b[v]->IsMatch);
    }
  }
}