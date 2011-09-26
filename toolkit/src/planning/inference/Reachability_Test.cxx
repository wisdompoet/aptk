#include <planning/inference/Reachability_Test.hxx>
#include <planning/Action.hxx>

namespace aig_tk
{

Reachability_Test::Reachability_Test( STRIPS_Problem& p )
	: m_problem( p )
{
	m_reachable_atoms.resize( m_problem.fluents().size() );
	m_action_mask.resize( m_problem.actions().size() );
}

Reachability_Test::~Reachability_Test()
{
}

void	Reachability_Test::initialize(Fluent_Vec& s)
{
	for ( unsigned i = 0; i < m_reachable_atoms.size(); i++ )
		m_reachable_atoms[i] = false;
	for ( unsigned i = 0; i < m_action_mask.size(); i++ )
		m_action_mask[i] = false;
	for ( unsigned i = 0; i < s.size(); i++ )
		m_reachable_atoms[ s[i] ] = true;
}

bool	Reachability_Test::apply_actions()
{
	bool fixed_point = true;

	for ( unsigned i = 0; i < m_problem.actions().size(); i++ )
	{
		if ( m_action_mask[i] ) continue;

		Action* a = m_problem.actions()[i];
		
		// Check if applicable
		Fluent_Vec&	pi = a->prec_vec();
		bool		applicable = true;
		for ( unsigned j = 0; j < pi.size(); j++ )
			if ( !m_reachable_atoms[pi[j]] )
			{
				applicable = false;
				break;
			}
		
		if ( !applicable ) continue;

		// Apply effects
		Fluent_Vec&	ai = a->add_vec();
		for ( unsigned j = 0; j < ai.size(); j++ )
		{
			if ( !m_reachable_atoms[ai[j]] )
			{
				m_reachable_atoms[ai[j]] = true;
				fixed_point = false;
			}
		}
		
		// Mark action as already used
		m_action_mask[i] = true;
	}

	return fixed_point;
}

bool	Reachability_Test::is_reachable( Fluent_Vec& s, Fluent_Vec& g )
{
	initialize(s);
	
	while ( !apply_actions() )
		if ( check( g ) )	
			return true;

	return check(g);
}

bool	Reachability_Test::is_reachable( Fluent_Vec& s, Fluent_Vec& g, unsigned op )
{
	initialize(s);

	m_action_mask[op] = true;	

	while ( !apply_actions() )
		if ( check( g ) )	
			return true;

	return check(g);
}


bool	Reachability_Test::check( Fluent_Vec& g )
{
	for ( unsigned i = 0; i < g.size(); i++ )
		if ( !m_reachable_atoms[ g[i] ] )
			return false;
	return true;
}

}
