MATCH (x0)<-[:plike]-()<-[:previewer]-()<-[:phasReview]-()-[:phomepage]->(x1), (x0)<-[:ppurchaseFor]-()-[:ppurchaseFor]->()<-[:plike]-()-[:phomepage]->(x2), (x0)-[:peditor]->()<-[:partist]-()-[:phomepage]->(x3), (x0)<-[:plike]-()-[:psubscribes]->(x4) RETURN "true" LIMIT 1;