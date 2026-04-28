% Family database and utility predicates

% male facts
male(brian).
male(matthew).
male(josiah).
male(tomjr).
male(tomsr).
male(richard).
male(richardjr).

% female facts
female(analise).
female(beverly).
female(anne).
female(june).
female(kristine).
female(sharon).

% parent facts
parent(brian,josiah).
parent(brian,analise).
parent(kristine,josiah).
parent(kristine,analise).
parent(tomjr,brian).
parent(tomjr,matthew).
parent(tomsr,tomjr).
parent(richard,beverly).
parent(richard,richardjr).
parent(richard,sharon).
parent(beverly,brian).
parent(beverly,matthew).
parent(anne,tomjr).
parent(june,beverly).
parent(june,richardjr).
parent(june,sharon).

% basic relations
father(X, Y) :- male(X), parent(X, Y).
mother(X, Y) :- female(X), parent(X, Y).
child(X, Y) :- parent(Y, X).
son(X, Y) :- child(X, Y), male(X).
daughter(X, Y) :- child(X, Y), female(X).

grandparent(X, Y) :- parent(X, Z), parent(Z, Y).
grandfather(X, Y) :- grandparent(X, Y), male(X).
grandmother(X, Y) :- grandparent(X, Y), female(X).

% sibling: share at least one parent, and are distinct
% sibling: share at least one parent, and are distinct
% use cut to avoid duplicate solutions when two parents are shared
sibling(X, Y) :- parent(P, X), parent(P, Y), X \= Y, !.

% aunt(X, Y) means X is an aunt of Y: X is female and a sibling of Y's parent
aunt(X, Y) :- female(X), parent(P, Y), sibling(X, P).

% uncle(X, Y) means X is an uncle of Y: X is male and a sibling of Y's parent
uncle(X, Y) :- male(X), parent(P, Y), sibling(X, P).

% list utilities: append/3 and reverse/2 for simple (non-nested) lists
append([], L, L).
append([H|T], L2, [H|R]) :- append(T, L2, R).

% reverse(R, L) means R is the reversed version of L
% implement with an accumulator: reverse(+L, -R) via rev_acc
reverse(R, L) :- rev_acc(L, [], R).

rev_acc([], Acc, Acc).
rev_acc([H|T], Acc, R) :- rev_acc(T, [H|Acc], R).
