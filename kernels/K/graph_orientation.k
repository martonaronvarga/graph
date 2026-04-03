/ graph_orientation.k
/ Main entry: solve[mode; n; bounds; eu; ev]
/ mode: 0=Edmonds-Karp, 1=Dinic, 2=Push-Relabel

/ Helper: build n x n capacity matrix with 1s for each edge
buildCap:{[n;eu;ev]
  m:count eu;
  mx:n#n#0;
  do[m; { mx[eu x; ev x]:mx[eu x; ev x]+1 } each 0+m#til m ];
  mx }

/ BFS for augmenting path
bfs:{[c;f;s;t]
  n:count c;
  pred:n#-1;
  q:enlist s;
  found:0;
  while[count q>0,not found;
    u:first q;
    q:q 1_;
    do[n; {if [pred[x]=-1; if [c[u;x]-f[u;x]>0; pred[x]:u; if[x=t; found:1; ]; q,:x ] ]} ];
  ];
  pred }

/ Reconstruct augmenting path
augPath:{[pred;s;t]
  p:enlist t;
  v:t;
  while[v<>s; v:pred[v]; p,:v ];
  reverse p }

/ Add flow along an augmenting path
addFlow:{[f;path;delta]
  do[count path-1; {
    u:path x; v:path x+1;
    f[u;v]+:delta;
    f[v;u]-:delta;
  }];
  f }

prior:{ x 0 _; til count x-1 }

minresdelta:{[c;f;path] min each {c[x;y]-f[x;y]} each prior path }

/ Edmonds-Karp implementation
ek:{[c;n;s;t]
  f:n#n#0;
  maxf:0;
  while[1;
    p:bfs[c;f;s;t];
    if[p[t]= -1; :f,maxf];
    path:augPath[p;s;t];
    delta:minresdelta[c;f;path];
    f:addFlow[f;path;delta];
    maxf+:delta;
  ];
  f,maxf }

/ Helper: Level BFS
levelBFS:{[c;f;s;t]
  n:count c;
  level:n#-1;
  q:enlist s;
  level[s]:0;
  while[count q>0;
    u:first q;
    q:q 1_;
    do[n;{ if[level[x]= -1, c[u;x]-f[u;x]>0; level[x]:level[u]+1; q,:x] }]
  ];
  level }

/ DFS for blocking flow
/ Returns: amount pushed, updated flow
dfsDinic:{[c;f;level;u;t;lim]
  if[u=t; :lim,f];
  n:count c;
  total:0;
  ff:f;
  do[n;
    v:x;
    if[level[v]=level[u]+1, c[u;v]-ff[u;v]>0;
      d:min[lim-total;c[u;v]-ff[u;v]];
      if[d>0;
        pushed,newf:dfsDinic[c;ff;level;v;t;d];
        if[pushed>0;
          ff[u;v]+:pushed;
          ff[v;u]-:pushed;
          total+:pushed;
          if[total=lim; :total,ff ] ;
        ]
      ]
    ]
  ];
  total,ff }

/ Main Dinic
dinic:{[c;n;s;t]
  f:n#n#0;
  maxf:0;
  while[1;
    level:levelBFS[c;f;s;t];
    if[level[t]= -1; :f,maxf];
    flowinc:0;
    while[1;
      pushed,newf:dfsDinic[c;f;level;s;t;1e9];
      if[pushed=0; :];
      flowinc+:pushed;
      f:newf
    ];
    if[flowinc=0; :f,maxf];
    maxf+:flowinc;
  ];
  f,maxf }

/ Push-Relabel helpers
/ Find admissible vertices
admissibleVs:{[c;f;h;u]
  n:count c;
  vs:();
  do[n;{if[c[u;x]-f[u;x]>0, h[u]=h[x]+1; vs,:x]}];
  vs }

/ push-relabel: classic FIFO variant
pushrelab:{[c;n;s;t]
  f:n#n#0;
  h:n#0; h[s]:n;
  excess:n#0;
  / Initialize preflow
  do[n;{if[c[s;x]>0;
    f[s;x]:c[s;x];
    f[x;s]:-c[s;x];
    excess[x]:c[s;x];
  ]}];
  q:();
  do[n;{if[(x<>s)&(x<>t)&(excess[x]>0); q,:x]}];
  while[count q>0;
    u:first q; q:q 1_;
    nsteps:0;
    while[excess[u]>0;
      vs:admissibleVs[c;f;h;u];
      if[count vs>0;
        do[count vs;{
          v:vs x;
          delta:min[excess[u]; c[u;v]-f[u;v]];
          if[delta>0;
            f[u;v]+:delta; f[v;u]-:delta;
            excess[u]-:delta; excess[v]+:delta;
            if[(v<>s)&(v<>t)&(excess[v]=delta); q,:v ];
          ]
        }]
      ;
        h[u]+:1;
      ];
      nsteps+:1;
      if[nsteps>2*n; :];
    ]
  ];
  f,excess[t] }

orientEdges:{[eu;ev;f]
  m:count eu;
  r:m#0;
  do[m; { u:eu x; v:ev x; if[f[u;v]>0; r[x]:0; r[x]:1] } ];
  r }

solve:{[mode;n;bounds;eu;ev]
  s:0;
  t:n-1;
  cap:buildCap[n;eu;ev];
  alg:(ek;dinic;pushrelab)[mode];
  f:maxf:alg[cap;n;s;t];
  orients:orientEdges[eu;ev;f];
  indeg:n#0;
  m:count eu;
  do[m; { u:eu x; v:ev x; if[orients[x]=0; indeg[v]+:1; else; indeg[u]+:1 ] } ];
  viol:where indeg>bounds;
  (maxf=m; orients; viol)
 }
