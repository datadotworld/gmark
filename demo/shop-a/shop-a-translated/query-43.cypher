MATCH (x0)<-[:planguage]-()<-[:psubscribes]-()-[:pgivenName]->(x1), (x1)<-[:pdescription]-()-[:peditor]->()-[:pbirthDate]->(x2), (x0)<-[:planguage]-()-[:ptext]->()<-[:ptext]-()-[:pprintPage]->(x3), (x2)<-[:planguage]-()<-[:ppurchaseFor]-()-[:ppurchaseFor]->()-[:pnumberOfPages]->(x4) RETURN "true" LIMIT 1;