//  GAMBIT: Global and Modular BSM Inference Tool
//  **********************************************
//
//  Dependency resolution with boost graph library
//
//  **********************************************
//
//  Authors
//  =======
//
//  (add name and date if you modify)
//
//  Christoph Weniger <c.weniger@uva.nl>
//  May, June, July 2013
//  Pat Scott 
//  May 03 2013
//
//  *********************************************

#include <vector>
#include <functors.hpp>
#include <graphs.hpp>
#include <yaml_parser.hpp>
#include "boost/format.hpp"
// #include <regex>
#include <boost/graph/graphviz.hpp>

#define OMEGA_VERTEXID 52314768

namespace GAMBIT
{
  namespace Graphs
  {
    //
    // Helper functions
    //

    // Collect parent vertices recursively (including root vertex)
    std::set<VertexID> getParentVertices(const VertexID & vertex, const
        Graphs::MasterGraphType & graph)
    {
      std::set<VertexID> myVertexList;
      myVertexList.insert(vertex);
      std::set<VertexID> parentVertexList;

      graph_traits<Graphs::MasterGraphType>::in_edge_iterator ibegin, iend;
      for (boost::tie(ibegin, iend) = in_edges(vertex, graph);
          ibegin != iend; ++ibegin)
      {
        parentVertexList = getParentVertices(source(*ibegin, graph), graph);
        myVertexList.insert(parentVertexList.begin(), parentVertexList.end());
      }
      return myVertexList;
    }

    // Get sorted list (according to topological sort result))
    std::vector<VertexID> getSortedParentVertices(const VertexID & vertex, const
        Graphs::MasterGraphType & graph, std::list<VertexID> topoOrder)
    {
      std::set<VertexID> set = getParentVertices(vertex, graph);
      std::vector<VertexID> result;
      for(std::list<VertexID>::iterator it = topoOrder.begin(); it != topoOrder.end(); it++)
      {
        if (set.find(*it) != set.end())
          result.push_back(*it);
      }
      return result;
    }

    // Return time estimate for set of nodes
    double getTimeEstimate(std::set<VertexID> vertexList, const Graphs::MasterGraphType &graph)
    {
      double result = 0;
      for (std::set<VertexID>::iterator it = vertexList.begin(); it != vertexList.end(); ++it)
      {
        result += graph[*it]->getRuntimeAverage();
      }
      return result;
    }

    // Compare two strings
    bool stringComp(str s1, str s2)
    {
      if ( s1 == s2 ) return true;
      if ( s1 == "" ) return true;
      if ( s1 == "*" ) return true;
      // if ( std::regex_match ( s2, *(new std::regex(s1)) ) ) return true; 
      return false;
    }

    bool quantityMatchesIniEntry(const sspair & quantity, const IniParser::ObservableType & observable)
    {
      // Compares dependency specifications of auxiliary entries or observable
      // entries with capability (capabilities have to be unique for these
      // lists))
      if ( stringComp( observable.capability, quantity.first ) ) return true;
      else return false;
    }

    bool funcMatchesIniEntry(functor *f, const IniParser::ObservableType &e)
    {
      if (     stringComp( e.capability, (*f).capability() )
           and stringComp( e.type, (*f).type() )
           and stringComp( e.function, (*f).name() )
           and stringComp( e.module, (*f).origin() ) )
           return true;
      else return false;
    }

    // Compare backend function with backend entry in inifile
    bool compareBE(IniParser::ObservableType observable, functor* func)
    {
      for (std::vector<IniParser::ObservableType>::iterator be =
          observable.backends.begin(); be != observable.backends.end(); be++)
      {
        // If capability matches...
        if ( (*be).capability == (*func).capability() )
        {
          // ...check function names
          if ( (*be).function != "" and (*be).function != (*func).name()
              ) return false;
          // ...check module name
          if ( (*be).module != "" and (*be).module != (*func).origin()
              ) return false;
          // ...check module version
          if ( (*be).version != "" and (*be).version != (*func).version()
              ) return false;
        }
      }
      return true; // everything consistent
    }

    // returns a list of backend functors which match in capability and type
    std::vector<functor *> findBackendCandidates(sspair key, std::vector<functor *> functorList)
    {
      std::vector<functor *> candidateList;
      for (unsigned int i=0; i<functorList.size(); ++i)
      {
        if ( functorList[i]->quantity() == key )
        {
          candidateList.push_back(functorList[i]);
        }
      }
      return candidateList;
    }

    class labelWriter
    {
      private:
        const Graphs::MasterGraphType * myGraph;
      public:
        labelWriter(const Graphs::MasterGraphType * masterGraph) : myGraph(masterGraph) {};
        void operator()(std::ostream& out, const VertexID& v) const
        {
	        out << "[fillcolor=\"#F0F0D0\", style=\"rounded,filled\", shape=box,";
          out << "label=< ";
          out << "<font point-size=\"20\" color=\"red\">" << (*myGraph)[v]->capability() << "</font><br/>";
          out <<  "Type: " << (*myGraph)[v]->type() << "<br/>";
          out <<  "Function: " << (*myGraph)[v]->name() << "<br/>";
          out <<  "Module: " << (*myGraph)[v]->origin();
          out << ">]";
        }
    };

    //
    // Public functions of DependencyResolver
    //

    // Constructor. 
    // Add module and backend functors to class internal lists.
    DependencyResolver::DependencyResolver(
        std::vector<functor *> functorList,
        std::vector<functor *> backendFunctorList, 
        IniParser::IniFile iniFile)
    {
      addFunctors(functorList, backendFunctorList);
      myIniFile = iniFile;
      addLegs();
    }

    // Main dependency resolution
    void DependencyResolver::resolveNow()
    {
      // (cap., typ) --> dep. vertex map
      std::queue<std::pair<sspair, Graphs::VertexID> > parQueue;
      std::pair<sspair, Graphs::VertexID> queueEntry;

      cout << endl << "Target likelihoods/observables" << endl;
      cout <<         "------------------------------" << endl;
      cout <<         "CAPABILITY (TYPE)"   << endl;
      for (IniParser::ObservablesType::const_iterator it =
          myIniFile.observables.begin(); it != myIniFile.observables.end(); ++it)
      {
        cout << (*it).capability << " (" << (*it).type << ")" << endl;
        queueEntry.first.first = (*it).capability;
        queueEntry.first.second = (*it).type;
        queueEntry.second = OMEGA_VERTEXID;
        parQueue.push(queueEntry);
      }
      generateTree(parQueue);
      function_order = run_topological_sort();

      // Generate graphviz plot
      std::ofstream outf("graph.gv");
      write_graphviz(outf, masterGraph, labelWriter(&masterGraph));
    }

    // List of masterGraph content
    void DependencyResolver::printFunctorList() 
    {
      graph_traits<Graphs::MasterGraphType>::vertex_iterator vi, vi_end;
      str formatString = "%-25s %-25s %-25s %-25s %-25s %-7i %-5i %-5i\n";
      int i = 0;
      cout << endl << "Vertices registered in masterGraph" << endl;
      cout << "----------------------------------" << endl;
      cout << boost::format(formatString)%
        "MODULE (VERSION)"% "FUNCTION"% "CAPABILITY"% "TYPE"% "OBSTYPE"% "STATUS"% "#DEPs"% "#BE_REQs";
      for (tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi) {
        cout << boost::format(formatString)%
          ((*masterGraph[*vi]).origin() + " (" + (*masterGraph[*vi]).version() + ")") %
          (*masterGraph[*vi]).name()%
          (*masterGraph[*vi]).capability()%
          (*masterGraph[*vi]).type()%
          (*masterGraph[*vi]).obsType()%
          (*masterGraph[*vi]).status()%
          (*masterGraph[*vi]).dependencies().size()%
          (*masterGraph[*vi]).backendreqs().size();
        i++;
      };
      // cout << "TOTAL: " << i << endl;
    };

    // New IO routines
    std::vector<VertexID> DependencyResolver::getObsLikeOrder()
    {
      std::vector<VertexID> unsorted;
      std::vector<VertexID> sorted;
      std::set<VertexID> parents, friends;
      // Copy unsorted vertexIDs --> unsorted
      for (std::vector<OutputVertexInfo>::iterator it = outputVertexInfos.begin();
          it != outputVertexInfos.end(); it++)
      {
        unsorted.push_back(it->vertex);
      }
      // Sort iteratively (unsorted --> sorted)
      while (unsorted.size() > 0)
      {
        double t2p_now;
        double t2p_min = -1;
        std::vector<VertexID>::iterator it_min;
        for (std::vector<VertexID>::iterator it = unsorted.begin(); it !=
            unsorted.end(); ++it)
        {
          parents = getParentVertices(*it, masterGraph);
          parents.insert(friends.begin(), friends.end()); // parents and friends
          t2p_now = (double) getTimeEstimate(parents, masterGraph);
          t2p_now /= masterGraph[*it]->getInvalidationRate();
          if (t2p_min < 0 or t2p_now < t2p_min)
          {
            t2p_min = t2p_now;
            it_min = it;
          }
        }
        double prop = masterGraph[*it_min]->getInvalidationRate();
        cout << "Estimated T [ns]: " << t2p_min*prop << endl;
        cout << "Estimated p: " << prop << endl;
        sorted.push_back(*it_min);
        unsorted.erase(it_min);
      }
      return sorted;
    }

    void DependencyResolver::calcObsLike(VertexID vertex)
    {
      std::vector<VertexID> order;
      // TODO: Should happen only once
      order = getSortedParentVertices(vertex, masterGraph, function_order);
      for (std::vector<VertexID>::iterator it = order.begin(); it != order.end(); ++it) {
        masterGraph[*it]->calculate();
      }
    }

    const IniParser::ObservableType * DependencyResolver::getIniEntry(VertexID vertex)
    {
      for(std::vector<OutputVertexInfo>::iterator it = outputVertexInfos.begin();
          it != outputVertexInfos.end(); it++)
      {
        if (it->vertex == vertex)
          return it->iniEntry;
      }
      cout << "ERROR" << endl;
      exit(1);
    }

    double DependencyResolver::getObsLike(VertexID vertex)
    {
      // Returns just doubles, and crashes for other types
      // TODO: Catch errors
      return (*(dynamic_cast<module_functor<double>*>(masterGraph[vertex])))();
    }

    void DependencyResolver::notifyOfInvalidation(VertexID vertex)
    {
      masterGraph[vertex]->notifyOfInvalidation();
    }

    void DependencyResolver::resetAll()
    {
      graph_traits<Graphs::MasterGraphType>::vertex_iterator vi, vi_end;
      for (tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi) 
      {
        masterGraph[*vi]->reset();
      }
    }

    //
    // Private functions of DependencyResolver
    //

    // Add alpha and omega legs to masterGraph
    void DependencyResolver::addLegs()
    {
      // Define alpha/omega vertices
      module_functor<double> * p_modfunc;

      // Input legs
      for (IniParser::ParametersType::const_iterator it =
          myIniFile.parameters.begin(); it != myIniFile.parameters.end(); ++it)
      {
        inputMap[(*it).name] = new double;
      }
      for (inputMapType::iterator it = inputMap.begin(); it != inputMap.end();
          ++it)
      {
        p_modfunc = new module_functor<double> (it->second, it->first +
            "_CoreIn", it->first, "double", "Core");
        boost::add_vertex(p_modfunc, masterGraph);
      }
    }

    // Add module and backend functors to class internal lists.
    void DependencyResolver::addFunctors(
        std::vector<functor *> functorList,
        std::vector<functor *> backendFunctorList )
    {
      // - module functors go into masterGraph
      // - backend functors go into myBackendFunctorList
      for (std::vector<functor *>::iterator it = functorList.begin();
          it != functorList.end(); ++it)
      {
        boost::add_vertex(*it, this->masterGraph);
      }
      this->myBackendFunctorList = backendFunctorList;
    }

    // Resolve dependency
    std::pair<const IniParser::ObservableType *, Graphs::VertexID>
      DependencyResolver::resolveDependency(
        Graphs::VertexID toVertex, sspair quantity)
    {
      graph_traits<Graphs::MasterGraphType>::vertex_iterator vi, vi_end;
      const IniParser::ObservableType *auxEntry, *depEntry;
      std::vector<Graphs::VertexID> vertexCandidates;
      bool entryExists = false;

      // Find inifile entry:
      // If toVertex is CoreOut vertex, use observable entries.
      if ( toVertex == OMEGA_VERTEXID)
      {
        depEntry = findIniEntry(quantity, myIniFile.observables);
        entryExists = true;
      }
      // for all other vertices.
      else 
      {
        auxEntry = findIniEntry(toVertex, myIniFile.auxiliaries);
        if ( auxEntry != NULL )
          depEntry = findIniEntry(quantity, (*auxEntry).dependencies);
        if ( auxEntry != NULL and depEntry != NULL ) 
          entryExists = true;
      }

      // Loop over available vertices (possibly matching inifile entry)
      for (tie(vi, vi_end) = vertices(masterGraph); vi != vi_end; ++vi) 
      {
        if ( ( (not entryExists)
               and (*masterGraph[*vi]).capability() == quantity.first
               and (*masterGraph[*vi]).type() == quantity.second )
          or ( entryExists
               and funcMatchesIniEntry(masterGraph[*vi], *depEntry) ) )
        {
          vertexCandidates.push_back(*vi);
        }
      }
      if ( vertexCandidates.size() == 0 ) 
      {
        cout << "ERROR: I could not find any module function that provides the requested" << endl;
        cout << "capability with the requested type. Check your inifile for typos, your modules" << endl;
        cout << "for consistency, etc." << endl;
        exit(0); // Throw error here
      }
      if ( vertexCandidates.size() > 1 ) 
      {
        cout << "ERROR: I found too many module functions that provide the requested" << endl;
        cout << "capability with the requested type. Check your inifile for typos, your modules" << endl;
        cout << "for consistency, etc." << endl;
        exit(0); // Throw error here
      }
      return std::make_pair(depEntry, vertexCandidates[0]);
    }

    // Set up dependency tree
    void DependencyResolver::generateTree(
        std::queue<std::pair<sspair, Graphs::VertexID> > parQueue)
    {
      OutputVertexInfo outInfo;
      Graphs::VertexID fromVertex, toVertex;
      Graphs::EdgeID edge;
      // relevant observable entry (could be dependency of another observable)
      IniParser::ObservableType observable, directObs;
      const IniParser::ObservableType * iniEntry;
      bool ok, obsFlag, dirObsFlag;
      dirObsFlag = false;
      sspair quantity;

      cout << endl << "Dependency resolution" << endl;
      cout <<         "---------------------" << endl;
      cout <<         "CAPABILITY (TYPE) [FUNCTION, MODULE]" << endl << endl;
      // Repeat until dependency queue is empty
      while (not parQueue.empty()) {
        // Retrieve capability, type and vertex ID of dependency of interest
        quantity = parQueue.front().first;
        toVertex = parQueue.front().second;

        // Print information
        if ( toVertex != OMEGA_VERTEXID )
        {
          cout << (*masterGraph[toVertex]).capability() << " (";
          cout << (*masterGraph[toVertex]).type() << ") [";
          cout << (*masterGraph[toVertex]).name() << ", ";
          cout << (*masterGraph[toVertex]).origin() << "]" << endl;
          cout << "depends on: ";
          cout << quantity.first << " (" << quantity.second << ")" << endl;
        }
        else
        {
          cout << "Core depends on: ";
          cout << quantity.first << " (" << quantity.second << ")" << endl;
        }

        // Resolve dependency
        tie(iniEntry, fromVertex) = resolveDependency(toVertex, quantity);

        // Print user info.
        cout << "resolved by: [";
        cout << (*masterGraph[fromVertex]).name() << ", ";
        cout << (*masterGraph[fromVertex]).origin() << "]" << endl;

        if ( toVertex != OMEGA_VERTEXID)
        {
          // Resolve dependency on functor level...
          (*masterGraph[toVertex]).resolveDependency(masterGraph[fromVertex]);
          // ...and on masterGraph level.
          tie(edge, ok) = add_edge(fromVertex, toVertex, masterGraph);
        }
        else
        {
          outInfo.vertex = fromVertex;
          outInfo.iniEntry = iniEntry;
          outputVertexInfos.push_back(outInfo);
        }

        // Is fromVertex already activated?
        if ( (*masterGraph[fromVertex]).status() != 2 ) {
          cout << "Adding module function to dependency tree..." << endl;
          // If not, perform resolution of backend dependencies...
          // TODO: rewrite
          resolveVertexBackend(dirObsFlag, directObs, fromVertex);
          // TODO: Treat other conditional dependencies
          // ...and fill its dependencies into queue.
          fillParQueue(&parQueue, fromVertex);
        }

        // Conclude.
        cout << endl;
        parQueue.pop();
      }
    }

    // Push module function dependencies on parameter queue
    void DependencyResolver::fillParQueue(
        std::queue<std::pair<sspair, Graphs::VertexID> > *parQueue,
        Graphs::VertexID vertex) 
    {
      (*masterGraph[vertex]).setStatus(2); // activate node, TODO: move somewhere else
      std::vector<sspair> vec = (*masterGraph[vertex]).dependencies();
      for (std::vector<sspair>::iterator it = vec.begin(); it != vec.end(); ++it) 
      {
        cout << (*it).first << " (" << (*it).second << ")" << endl;
        (*parQueue).push(*(new std::pair<sspair, Graphs::VertexID> (*it, vertex)));
      }
    }

    // Boost lib topological sort
    std::list<VertexID> DependencyResolver::run_topological_sort()
    {
      std::list<VertexID> topo_order;
      topological_sort(masterGraph, front_inserter(topo_order));
      return topo_order;
    }

    // Find auxiliary entry that matches vertex
    const IniParser::ObservableType * DependencyResolver::findIniEntry(
        Graphs::VertexID toVertex,
        const IniParser::ObservablesType &entries)
    {
      std::vector<const IniParser::ObservableType*> auxEntryCandidates;
      for (IniParser::ObservablesType::const_iterator it =
          entries.begin(); it != entries.end(); ++it)
      {
        if ( funcMatchesIniEntry(masterGraph[toVertex], *it ) )
        {
          auxEntryCandidates.push_back(&(*it));
        }
      }
      if ( auxEntryCandidates.size() == 0 ) return NULL;
      if ( auxEntryCandidates.size() == 1 ) return auxEntryCandidates[0];
      else
      {
        cout << "ERROR: Multiple matching auxiliary entries found for same vertex." << endl;
        exit(0);
      }
    }

    // Find observable entry that matches capability/type
    const IniParser::ObservableType *DependencyResolver::findIniEntry(
        sspair quantity, const IniParser::ObservablesType &entries)
    {
      std::vector<const IniParser::ObservableType*> obsEntryCandidates;
      for (IniParser::ObservablesType::const_iterator it =
          entries.begin(); it != entries.end(); ++it)
      {
        if ( quantityMatchesIniEntry(quantity, *it) ) // use same criteria than for normal dependencies
        {
          obsEntryCandidates.push_back(&(*it));
        }
      }
      if ( obsEntryCandidates.size() == 0 ) return NULL;
      if ( obsEntryCandidates.size() == 1 ) return obsEntryCandidates[0];
      else
      {
        cout << "ERROR: Multiple matches for identical capability in inifile." << endl;
        cout << "Capability: " << quantity.first << " (" << quantity.second << ")" << endl;
        exit(0);
      }
    }

    // Node-by-node backend resolution
    void DependencyResolver::resolveVertexBackend(bool dirObsFlag,
        IniParser::ObservableType observable, VertexID vertex)
    {
      std::vector<sspair> reqs;
      std::vector<functor *> candidateList;
      functor * candidate;
      // Get list of backend requirements of vertex
      reqs = (*masterGraph[vertex]).backendreqs();
      if (reqs.size() != 0)
      {
        cout << "Backend resolution for vertex " << (*masterGraph[vertex]).name() << endl;
        for (std::vector<sspair>::iterator it = reqs.begin();
            it != reqs.end(); ++it)
        {
          cout << "  " << it->first << " (" << it->second << ")" << endl;
          // Find backend function candidates (by matching capabilities and
          // types)
          candidateList = findBackendCandidates(*it, myBackendFunctorList);
          // No candidate found, that is bad news.
          if (candidateList.size() == 0)
          {
            cout << "no candidate found" << endl;
            exit(0);
          }
          // One candidate...
          if (candidateList.size() == 1)
          {
            // VERBOSE
            // cout << "one candidate found" << endl;
            // ...but is it the choosen one?
            if (dirObsFlag)
            {
              if (not compareBE(observable, candidateList[0]))
              {
                cout << "Inconsistent BE infos." << endl;
                exit(0);
              }
            }
            candidate = candidateList[0];
          }
          // Multiple options, nice.
          if (candidateList.size() > 1)
          {
            // loop over options
            int i = 0;
            for(std::vector<functor *>::iterator it = candidateList.begin();
                it != candidateList.end(); it++)
            {
              // if some option works
              if (compareBE(observable, *it))
              {
                // save it and proceed
                candidate = *it;
                i++;
              }
            }
            if (i!=1) // There can be only one!
            {
              cout << "Multiple backend options, but no unique solution." << endl;
              exit(0);
            }
          }
          // Resolve it
          (*masterGraph[vertex]).resolveBackendReq(candidate);
          cout << "  resolved by: " << (*candidate).name();
          cout << " from " << (*candidate).origin() << " (";
          cout << (*candidate).version() << ")" << endl;
        }
      }
      // VERBOSE
      // else cout << "no backends to resolve" << endl;
    }
  }
}