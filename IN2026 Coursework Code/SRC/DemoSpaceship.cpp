#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "DemoSpaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
DemoSpaceship::DemoSpaceship()
	: GameObject("DemoSpaceship"), mThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
DemoSpaceship::DemoSpaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("DemoSpaceship", p, v, a, h, r), mThrust(0)
{
}

/** Copy constructor. */
DemoSpaceship::DemoSpaceship(const DemoSpaceship& s)
	: GameObject(s), mThrust(0)
{
}

/** Destructor. */
DemoSpaceship::~DemoSpaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void DemoSpaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);
}

/** Render this spaceship. */
void DemoSpaceship::Render(void)
{
	if (mDemoSpaceshipShape.get() != NULL) mDemoSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mDemoThrusterShape.get() != NULL)) {
		mDemoThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. */
void DemoSpaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void DemoSpaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. */
void DemoSpaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the demo mode spaceship is headed
	GLVector3f demo_spaceship_heading(cos(DEG2RAD*mAngle), sin(DEG2RAD*mAngle), 0);
	demo_spaceship_heading.normalize();
	// Calculate the point at the node of the demo mode spaceship from position and heading
	GLVector3f bullet_position = mPosition + (demo_spaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + demo_spaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
		(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mDemoBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

bool DemoSpaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void DemoSpaceship::OnCollision(const GameObjectList &objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}