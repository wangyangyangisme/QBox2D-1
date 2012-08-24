#include "arcanoidworld.h"

ArcanoidWorld::ArcanoidWorld() :
    QBox2DWorld()
{

}

void ArcanoidWorld::step(){
    QBox2DWorld::step();

    if (_contacts.size() == 0) {
        return;
    }

    QSet<QBox2DItem*> destroy_items;
    for(uint i = 0; i < (uint)_contacts.size() ; ++i){
        ContactPoint cp = _contacts.at(i);
        QBox2DItem *item = ( cp.itemA == _ball ) ? cp.itemB :
                           ( cp.itemB == _ball ) ? cp.itemA : NULL;
        if (!item) continue;

        if (item == _paddle) continue;
        if (item->bodyType() == b2_dynamicBody) {
            destroy_items.insert(item);
        } else if (item == _bound) {
            destroyItem(_ball);
            createBall();
               // Add game over here
        }
    }

    QSetIterator<QBox2DItem*> i(destroy_items);
    while(i.hasNext()){
        QBox2DItem *item = i.next();
        destroyItem(item);
    }
}

void ArcanoidWorld::create() {
    qDebug()<<"Enter in Create Arcanoid World";
        _world->SetGravity(b2Vec2(0, -1));

        QBox2DRectItem* groundl = new QBox2DRectItem();
        groundl->setPos(-205, 0);
        groundl->setBrush(QColor(128, 128, 128));
        groundl->setRestitution(1.0f);
        groundl->setBodyType(b2_staticBody);
        groundl->create(_world);
        groundl->setShape(QRectF(0, 0, 5, 400));
        emit itemCreated(groundl);

        QBox2DRectItem* groundr = new QBox2DRectItem();
        groundr->setPos(200, 0);
        groundr->setBrush(QColor(128, 128, 128));
        groundr->setRestitution(1.0f);
        groundr->setBodyType(b2_staticBody);
        groundr->create(_world);
        groundr->setShape(QRectF(0, 0, 5, 400));
        emit itemCreated(groundr);

        QBox2DRectItem* groundup = new QBox2DRectItem();
        groundup->setPos(-200, 0);
        groundup->setBrush(QColor(128, 128, 128));
        groundup->setBodyType(b2_staticBody);
        groundup->create(_world);
        groundup->setShape(QRectF(0, 0, 400, 5));
        emit itemCreated(groundup);

        QBox2DRectItem* hor_item = new QBox2DRectItem();
        hor_item->setPos(0, 400);
        hor_item->setBrush(QColor(128, 128, 128));
        hor_item->setBodyType(b2_dynamicBody);
        hor_item->setDensity(0.0f);
        hor_item->create(_world);
        hor_item->setShape(QRectF(0, 0, 5, 5));
        emit itemCreated(hor_item);

        _paddle = new QBox2DRectItem();
        _paddle->setPos(-50, 400 - 5);
        _paddle->setBrush(QColor(128, 128, 128));
        _paddle->setBodyType(b2_dynamicBody);
        _paddle->setFriction(1.0f);
        _paddle->setDensity(1.0f);
        _paddle->setRestitution(1.5f);
        _paddle->create(_world);
        _paddle->setShape(QRectF(0, 0, 100, 10));
        _paddle->body()->SetLinearDamping(10.0f);
        //_paddle->body()->SetType(b2_staticBody);
        emit itemCreated(_paddle);

        b2PrismaticJointDef hor_joint_def;
        b2Vec2 axis(1.0f, 0.0f);

        hor_joint_def.Initialize(hor_item->body(), _groundBody, b2Vec2(0,0), axis);
        hor_joint_def.lowerTranslation = -2.96f;
        hor_joint_def.upperTranslation = 2.96f;
        hor_joint_def.enableLimit = true;
        _world->CreateJoint(&hor_joint_def);

        b2PrismaticJointDef vert_joint_def;
        axis = b2Vec2(0.0f, 1.0f);

        vert_joint_def.Initialize(_paddle->body(), hor_item->body(), b2Vec2(0,0), axis);
        vert_joint_def.lowerTranslation = -0.5f;
        vert_joint_def.upperTranslation = 0.0f;
        vert_joint_def.enableLimit = true;
        _world->CreateJoint(&vert_joint_def);

        createBall();

        for (int j = 0; j < 10; ++j){
            for (int i = 0; i < 10; ++i){
                QBox2DRectItem* brick = new QBox2DRectItem();
                brick->setPos(-150 + 30*i, 10 + 30*j);
                brick->setBrush(QColor(128 + qrand() % 128, 128 + qrand() % 128, 128 + qrand() % 128));
                brick->setRestitution(1.0f);
                brick->setDensity(1.0f);
                brick->setBodyType(b2_dynamicBody);
                brick->create(_world);
                brick->setShape(QRectF(0, 0, 10, 20));
                emit itemCreated(brick);

                b2RevoluteJointDef jointDef;
                jointDef.Initialize(brick->body(), _groundBody, brick->body()->GetWorldCenter());
                jointDef.enableMotor = true;
                jointDef.motorSpeed = pow(-1,i+j) * (qrand() % 100) / 10;
                jointDef.maxMotorTorque = 5000.0f;
                _world->CreateJoint(&jointDef);
            }
        }

        _bound = new QBox2DRectItem();
        _bound->setPos(-205, 410);
        _bound->setBodyType(b2_staticBody);
        _bound->create(_world);
        _bound->setShape(QRectF(0, 0, 410, 10));
    }


void ArcanoidWorld::handleKeyPressed(const int &key)
{
    switch( key ) {
    case Qt::Key_W:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(0.0, 5.0f),_paddle->body()->GetWorldCenter(), true);
        break;
    case Qt::Key_S:
        _paddle->body()->ApplyLinearImpulse(b2Vec2(0.0, -5.0f),_paddle->body()->GetWorldCenter(), true);
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

void ArcanoidWorld::createBall(quint8 radius){
        QBox2DCircleItem *item = new QBox2DCircleItem();
        item->setPos(0, 200);
        item->setBrush(QColor(128 + qrand() % 128, 128 + qrand() % 128, 128 + qrand() % 128));
        item->setBodyType(b2_dynamicBody);
        item->setFriction(1.0f);
        item->setDensity(1.0f);
        item->setRestitution(1.0f);
        item->create(_world);
        item->setShape(QRectF(-radius , -radius, radius*2, radius*2));
        item->body()->SetBullet(true);
        _ball = item;
        emit itemCreated(item);
}

void ArcanoidWorld::createBall() {
    createBall(5);
}
