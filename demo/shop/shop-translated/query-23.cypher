MATCH (x0)<-[:p53]-()<-[:p57]-()<-[:p80]-()<-[:p81]-(x1), (x1)-[:p65|p65|p65*]->(x2), (x2)-[:p65]->()-[:p59]->()<-[:p67]-()<-[:p76]-(x3), (x3)-[:p76*]->(x4) RETURN DISTINCT x0, x4;