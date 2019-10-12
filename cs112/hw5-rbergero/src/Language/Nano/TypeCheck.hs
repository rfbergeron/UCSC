{-# LANGUAGE FlexibleInstances, OverloadedStrings, BangPatterns #-}

module Language.Nano.TypeCheck where

import Language.Nano.Types
import Language.Nano.Parser

import qualified Data.List as L
import           Text.Printf (printf)  
import           Control.Exception (throw)

--------------------------------------------------------------------------------
typeOfFile :: FilePath -> IO Type
typeOfFile f = readFile f >>= typeOfString

typeOfString :: String -> IO Type
typeOfString s = typeOfExpr (parseExpr s)

typeOfExpr :: Expr -> IO Type
typeOfExpr e = do
  let (!st, t) = infer initInferState preludeTypes e
  if (length (stSub st)) < 0 then throw (Error ("count Negative: " ++ show (stCnt st)))
  else return t

--------------------------------------------------------------------------------
-- Problem 1: Warm-up
--------------------------------------------------------------------------------

-- | Things that have free type variables
class HasTVars a where
  freeTVars :: a -> [TVar]

-- | Type variables of a type
instance HasTVars Type where
  freeTVars TInt = []
  freeTVars TBool = []
  freeTVars (TVar s) = [s]
  freeTVars (t1 :=> t2) = (freeTVars t1) ++ (freeTVars t2)
  freeTVars (TList t) = freeTVars t

-- | Free type variables of a poly-type (remove forall-bound vars)
instance HasTVars Poly where
  freeTVars (Forall v poly) = filter checkFree (freeTVars poly)
    where
      checkFree :: TVar -> Bool
      checkFree v1 = not (v1 == v)
  freeTVars (Mono typ) = freeTVars typ



-- | Free type variables of a type environment
instance HasTVars TypeEnv where
  freeTVars gamma   = concat [freeTVars s | (x, s) <- gamma]  
  
-- | Lookup a variable in the type environment  
lookupVarType :: Id -> TypeEnv -> Poly
lookupVarType x ((y, s) : gamma)
  | x == y    = s
  | otherwise = lookupVarType x gamma
lookupVarType x [] = throw (Error ("unbound variable: " ++ x))

-- | Extend the type environment with a new biding
extendTypeEnv :: Id -> Poly -> TypeEnv -> TypeEnv
extendTypeEnv x s gamma = (x,s) : gamma  

-- | Lookup a type variable in a substitution;
--   if not present, return the variable unchanged
lookupTVar :: TVar -> Subst -> Type
lookupTVar a []         = TVar a
lookupTVar a (sub:subs) = if (fst sub) == a then (snd sub) else lookupTVar a subs

-- | Remove a type variable from a substitution
removeTVar :: TVar -> Subst -> Subst
removeTVar a sub = filter (\(var,typ) -> not (var == a)) sub
     
-- | Things to which type substitutions can be apply
class Substitutable a where
  apply :: Subst -> a -> a
  
-- | Apply substitution to type
instance Substitutable Type where  
  apply sub TInt        = TInt
  apply sub TBool       = TBool
  apply sub (t1 :=> t2) = ((apply sub t1) :=> (apply sub t2))
  apply sub (TVar t)    = lookupTVar t sub
  apply sub (TList t)   = (TList (apply sub t))

-- | Apply substitution to poly-type
instance Substitutable Poly where    
  apply sub (Mono t)    = Mono (apply sub t)
  apply sub (Forall var poly) = Forall var (apply (removeTVar var sub) poly)

-- | Apply substitution to (all poly-types in) another substitution
instance Substitutable Subst where  
  apply sub to = zip keys $ map (apply sub) vals
    where
      (keys, vals) = unzip to
      
-- | Apply substitution to a type environment
instance Substitutable TypeEnv where  
  apply sub gamma = zip keys $ map (apply sub) vals
    where
      (keys, vals) = unzip gamma
      
-- | Extend substitution with a new type assignment
extendSubst :: Subst -> TVar -> Type -> Subst
extendSubst sub t a = (t,a):(zip tvs newTypes)
  where
    (tvs,tps) = unzip sub
    newTypes = map (apply [(t,a)]) tps

      
--------------------------------------------------------------------------------
-- Problem 2: Unification
--------------------------------------------------------------------------------
      
-- | State of the type inference algorithm      
data InferState = InferState { 
    stSub :: Subst -- ^ current substitution
  , stCnt :: Int   -- ^ number of fresh type variables generated so far
} deriving Show

-- | Initial state: empty substitution; 0 type variables
initInferState = InferState [] 0

-- | Fresh type variable number n
freshTV n = TVar $ "a" ++ show n      
    
-- | Extend the current substitution of a state with a new type assignment   
extendState :: InferState -> TVar -> Type -> InferState
extendState (InferState sub n) a t = InferState (extendSubst sub a t) n
        
-- | Unify a type variable with a type; 
--   if successful return an updated state, otherwise throw an error
unifyTVar :: InferState -> TVar -> Type -> InferState
unifyTVar st a TBool = extendState st a TBool
unifyTVar st a TInt = extendState st a TInt
unifyTVar st a (TVar b) = if a == b then st
  else extendState st a (TVar b)
unifyTVar st a (TList t) = if elem a (freeTVars t) then
  throw (Error ("type error: cannot unify " ++ (show a) ++ " and " ++ (show t)))
  else extendState st a (TList t)
unifyTVar st a (t1 :=> t2) = if (elem a (freeTVars t1)) || (elem a (freeTVars t2))
  then throw (Error ("type error: cannot unify " ++ (show a) ++ " and " ++ (show (t1 :=> t2))))
  else extendState st a (t1 :=> t2)
    
-- | Unify two types;
--   if successful return an updated state, otherwise throw an error
unify :: InferState -> Type -> Type -> InferState
unify st TInt TInt = st
unify st TBool TBool = st
unify (InferState sub n) (t1 :=> t2) (t3 :=> t4) = InferState (L.union sub13 sub24) n
  where
    (InferState sub13 n13) = unify (InferState sub n) t1 t3
    (InferState sub24 n24) = unify (InferState sub n) t2 t4
unify st t1 (t2 :=> t3) = unify st t1 t3
unify st (t1 :=> t2) t3 = unify st t1 t3
unify st (TVar a) t = unifyTVar st a t
unify st t (TVar a) = unifyTVar st a t
unify st (TList t1) (TList t2) = unify st t1 t2
unify st t1 t2 = throw (Error ("type error: could not unify " ++ (show t1) ++ " " ++ (show t2)))

--------------------------------------------------------------------------------
-- Problem 3: Type Inference
--------------------------------------------------------------------------------    
  
infer :: InferState -> TypeEnv -> Expr -> (InferState, Type)
infer st _   (EInt _)          = (st,TInt)
infer st _   (EBool _)         = (st,TBool)
infer (InferState sub n) gamma (EVar x)        = ((InferState sub n'),xT)
  where
    (n',xT) = instantiate n (lookupVarType x gamma)
infer (InferState sub n) gamma (ELam x body)   = (st',(tX :=> tBody))
  where
    gamma' = extendTypeEnv x (Mono tX) gamma
    tX = freshTV n
    (st',tBody) = infer (InferState sub (n + 1)) gamma' body
infer st gamma (EApp e1 e2)    = ((InferState subf nf),(lookupTVar (fsh) subf))
  where
    (st',t1) = infer st gamma e1
    ((InferState sub'' n''),t2) = infer st' gamma e2
    (TVar fsh) = freshTV n''
    (InferState subf nf) = unify (InferState sub'' (n'' + 1)) t1 (t2 :=> (TVar fsh))
infer st gamma (ELet x e1 e2)  = infer st' (gamma') e2
  where
    gamma' = extendTypeEnv x (generalize gamma t1) gamma
    (st',t1) = infer st gamma e1
infer st gamma (EBin op e1 e2) = infer st gamma asApp
  where
    asApp = EApp (EApp opVar e1) e2
    opVar = EVar (show op)
infer st gamma (EIf c e1 e2) = infer st gamma asApp
  where
    asApp = EApp (EApp (EApp ifVar c) e1) e2
    ifVar = EVar "if"    
infer st gamma ENil = infer st gamma (EVar "[]")

-- | Generalize type variables inside a type
generalize :: TypeEnv -> Type -> Poly
generalize gamma TInt = Mono TInt
generalize gamma TBool = Mono TBool
generalize gamma (t1 :=> t2) = L.foldl' (\x y -> Forall y x) (Mono (t1 :=> t2)) freevars
  where
    gvars = freeTVars gamma
    tvars = L.union (freeTVars t1) (freeTVars t2)
    freevars = tvars L.\\ gvars
generalize gamma (TVar a) = Mono (TVar a)
generalize gamma (TList t) = L.foldl' (\x y -> Forall y x) (Mono (TList t)) freevars
  where
    gvars = freeTVars gamma
    tvars = freeTVars t
    freevars = tvars L.\\ gvars
    
-- | Instantiate a polymorphic type into a mono-type with fresh type variables
instantiate :: Int -> Poly -> (Int, Type)
instantiate n (Mono t) = (n,t)
instantiate n (Forall a t) = (n', apply ([(a,(freshTV n))]) t')
  where
    (n', t') = instantiate (n + 1) t
      
-- | Types of built-in operators and functions      
preludeTypes :: TypeEnv
preludeTypes =
  [ ("+",    Mono $ TInt :=> TInt :=> TInt)
  , ("-",    Mono $ TInt :=> TInt :=> TInt)
  , ("*",    Mono $ TInt :=> TInt :=> TInt)
  , ("/",    Mono $ TInt :=> TInt :=> TInt)
  --, ("==",   error "TBD: ==")
  --, ("!=",   error "TBD: !=")
  --, ("<",    error "TBD: <")
  --, ("<=",   error "TBD: <=")
  --, ("&&",   error "TBD: &&")
  --, ("||",   error "TBD: ||")
  --, ("if",   error "TBD: if")
  -- lists: 
  --, ("[]",   error "TBD: []")
  --, (":",    error "TBD: :")
  --, ("head", error "TBD: head")
  --, ("tail", error "TBD: tail")
  ]
