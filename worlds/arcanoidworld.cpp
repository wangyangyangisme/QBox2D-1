#include "arcanoidworld.h"

ArcanoidWorld::ArcanoidWorld(QObject *parent) : QBox2DWorld(parent) {

}

void ArcanoidWorld::step(){
    QBox2DWorld::step();

    if (_contacts.size() == 0) {
        return;
    }

    b2Vec2 velocity = _ball->body()->GetLinearVelocity();
    float32 speed = velocity.Length();
    float32 maxSpeed = 50;

     if (speed > maxSpeed) {
         qDebug() << "Damping speed: " << speed;
         _ball->body()->SetLinearDamping(0.5);
     } else if (speed <= maxSpeed) {
         _ball->body()->SetLinearDamping(0.0);
         if (speed < 10){
             qDebug() << "Speed up: " << speed;
             _ball->body()->ApplyLinearImpulse(b2Vec2(10,-30), _ball->body()->GetWorldCenter(),true);
         }
     }

    QSet<QBox2DItem*> destroyItems;
    for(int i = 0; i < _contacts.size() ; ++i){
        ContactPoint cp = _contacts.at(i);
        QBox2DItem *item = ( cp.itemA == _ball ) ? cp.itemB :
                           ( cp.itemB == _ball ) ? cp.itemA : NULL;

        if (!item) continue;

        if (item == _paddle) continue;
        if (item->bodyType() == b2_dynamicBody) {
            destroyItems.insert(item);
        } else if (item == _bound) {
            destroyItem(_ball);
            createBall();
               // Add game over here
        }
    }

    QSetIterator<QBox2DItem*> i(destroyItems);
    while(i.hasNext()){
        QBox2DItem *item = i.next();
        destroyItem(item);
    }
}

void ArcanoidWorld::populate() {
    qDebug()<< "Starting Arcanoid World";

    loadWorld("data/levels/room.xml");
    _world->SetGravity(b2Vec2(0,0));

    float32 fieldSize = 200;
    QColor gray = QColor(128, 128, 128);
    b2PolygonShape shape;

    createBall();

    QBox2DItem* horItem = new QBox2DItem();
    horItem->setPos(b2Vec2(WSCALE2(0, fieldSize)));
    horItem->setBodyType(b2_dynamicBody);
    horItem->setDensity(0.0f);
    horItem->createBody(_world);
    shape.SetAsBox(WSCALE2(1,1));
    horItem->setShape(shape);
    horItem->setColor(gray);
    appendItem(horItem);


    _paddle = new QBox2DItem();
    _paddle->setPos(b2Vec2(WSCALE2(0, fieldSize)));
    _paddle->setBodyType(b2_dynamicBody);
    _paddle->setFriction(0.4f);
    _paddle->setDensity(10.0f);
    _paddle->setRestitution(0.1f);
    _paddle->createBody(_world);
    shape.SetAsBox(WSCALE2(50,5));
    _paddle->setShape(shape);
    _paddle->setUserData(_paddle);
    _paddle->body()->SetLinearDamping(10.0f);
    _paddle->setColor(gray);
    _paddle->_textureName = "kde.png";
    appendItem(_paddle);

    b2PrismaticJointDef horJointDef;
    b2Vec2 axis(1.0f, 0.0f);

    horJointDef.Initialize(horItem->body(), _ground, b2Vec2(0,0), axis);
    horJointDef.lowerTranslation = WSCALE(-fieldSize);
    horJointDef.upperTranslation = WSCALE(fieldSize);
    horJointDef.enableLimit = true;
    _world->CreateJoint(&horJointDef);

    b2PrismaticJointDef vertJointDef;
    axis = b2Vec2(0.0f, 1.0f);

    vertJointDef.Initialize(_paddle->body(), horItem->body(), b2Vec2(0,0), axis);
    vertJointDef.lowerTranslation = 0.0f;
    vertJointDef.upperTranslation = WSCALE(20.0f);
    vertJointDef.enableLimit = true;
    _world->CreateJoint(&vertJointDef);

    int n = 10;
    int brickWidth = 20;
    int xStep = fieldSize/n + brickWidth;
    int yStep = fieldSize/n + brickWidth;
    for (int j = 0; j < n-1; ++j){
        for (int i = 0; i < n; ++i){
            QBox2DItem* brick = new QBox2DItem();
            int xPos = i * xStep - fieldSize + brickWidth;
            int yPos = j * yStep - fieldSize + brickWidth;
            brick->setPos(b2Vec2(WSCALE2(xPos,yPos)));
            brick->setRestitution(0.1f);
            brick->setDensity(10.0f);
            brick->setBodyType(b2_dynamicBody);
            brick->createBody(_world);
            b2PolygonShape brickShape;
            brickShape.SetAsBox(WSCALE2(5,brickWidth/2));
            brick->setShape(brickShape);
            brick->setColor(QColor(128 + qrand() % 128, 128 + qrand() % 128, 128 + qrand() % 128));
            brick->body()->SetUserData(brick);
            brick->_textureName = "exit.png";
            appendItem(brick);

            b2RevoluteJointDef jointDef;
            jointDef.Initialize(brick->body(), _ground, brick->body()->GetWorldCenter());
            jointDef.enableMotor = true;
            jointDef.motorSpeed = pow(-1.0f,i+j) * (qrand() % 100) / 10;
            jointDef.maxMotorTorque = 5000.0f;
            _world->CreateJoint(&jointDef);
        }
    }
    _bound = new QBox2DItem();
    _bound->setPos(b2Vec2(WSCALE2(0, (fieldSize+20))));
    _bound->setBodyType(b2_staticBody);
    _bound->createBody(_world);
    shape.SetAsBox(WSCALE2(fieldSize,5));
    _bound->setShape(shape);
    _bound->body()->SetUserData(_bound);
    //appendItem(_bound);

}


void ArcanoidWorld::handleKeyPressed(const int &key)
{
    switch( key ) {
    case Qt::Key_W:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(0.0, -5.0f),_paddle->body()->GetWorldCenter(), true);
        break;
    case Qt::Key_S:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(0.0, 5.0f),_paddle->body()->GetWorldCenter(), true);
        break;
    case Qt::Key_A:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(-10.0f,0.0f),_paddle->body()->GetWorldCenter(), true);
        break;
    case Qt::Key_D:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(10.0f,0.0f),_paddle->body()->GetWorldCenter(), true);
        break;
    case Qt::Key_Q:
        _paddle->body()->ApplyTorque(1000,false);
        break;
    case Qt::Key_E:
        _paddle->body()->ApplyTorque(-1000, false);
        break;
    }
}

void ArcanoidWorld::createBall(float32 radius){
    QBox2DItem *ball = new QBox2DItem();
    ball->setBodyType(b2_dynamicBody);
    ball->setPos(b2Vec2(WSCALE2(0, 100)));
    ball->setFriction(0.f);
    ball->setDensity(1.0f);
    ball->setRestitution(1.1f);
    ball->createBody(_world);

    b2CircleShape circle;
    circle.m_radius = radius;
    ball->setShape(circle);
    ball->setColor(QColor(128 + qrand() % 128, 128 + qrand() % 128, 128 + qrand() % 128));
    ball->body()->SetUserData(ball);
    ball->body()->SetBullet(true);
    ball->_textureName = "ball.png";
    ball->setName("ball");
    appendItem(ball);
    _ball = ball;
    _ball->body()->ApplyLinearImpulse(b2Vec2(10,30), _ball->body()->GetWorldCenter(),true);
}

void ArcanoidWorld::createBall() {
    createBall(WSCALE(7));
}
