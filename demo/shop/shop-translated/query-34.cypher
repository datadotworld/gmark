MATCH (x0)<-[:p52]-()<-[:p57]-()-[:p62]->(x1), (x1)<-[:p62]-()-[:p57]->(x2), (x2)-[:p70]->()-[:p42]->(x3) RETURN DISTINCT x0;