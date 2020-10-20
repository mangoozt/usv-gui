#ifndef USV_DEFINES_H
#define USV_DEFINES_H

#include "Geometry.h"
#include <vector>

#define MAX_EXECUTION_TIME 30 //seconds
#define DELTA_T 9.12694019257921765732

namespace USV {
    /**
     * \brief Категория судна в контексте правила 18 МППСС-72
     */
    enum ShipCategory {
        SCPropelled = 0,     //! 0 - судно, оборудованное механическим двигателем;
        SCSailing = 1,       //! 1 - парусное судно;
        SCFishing = 2,       //! 2 - судно, занятое ловом рыбы;
        SCSteerImpaired = 3, //! 3 - судно, ограниченное в возможности маневрировать;
        SCUnmanned = 4,      //! 4 - судно, лишенное возможности управления;
        SCDraft = 5,         //! 5 - судно, ограниченное своей осадкой;
        SCFollowingTSS = 6,  //! 6 - судно, оборудованное механическим двигателем, следующее по СРД
    };

    enum class ScenarioTypeTSS {
        Sct0_None = 0,
        Sct1_Cross,
        Sct2_Entering
    };
    /**
     * \brief Тип сценари¤ расхождения согласно ТЗ
     */
    enum class ScenarioType {
        Sc0_None = 0,
        Sc1_FaceToFace,
        Sc2_Overtaken,
        Sc3_Overtake,
        Sc4_GiveWay,
        Sc5_Save,
        Sc6_GiveWayPriority,
        Sc7_SavePriority,
        Sc8_CrossMove,
        Sc9_CrossIn,
        Sc10_VisionRestricted_Fwd,
        Sc11_VisionRestricted_Bwd
    };

    /**
     * \brief Тип зоны расхождения
     */
    enum class ZoneType {
        DangerNone = 0,
        Capture_7_to_12,
        Decision_5_to_7,
        Control_2_to_5,
        Close_less_2
    };

    /**
     * \brief Тип опасности судна
     */
    enum class DangerType {
        NotDangerous = 0,
        PotentiallyDangerous,
        Dangerous
    };

    struct DangerousVehicle {
        size_t index;
        DangerType dangerType;
        double distance;
        double CPA;
        double TCPA;
    };

    struct SafeZone {
        int safety_zone_type;

        double radius;
        double start_angle;
        double end_angle;
        double length;
        double width;
        double lengthOffset;
    };
    enum class ViolatedType {
        False = 0,
        True = 1,
        None = -1
    };

    enum class RestrictionType {
        Hard,
        Soft,
        None
    };

    enum class LimitationType {
        point_approach_prohibition = 0,
        line_crossing_prohibition,
        zone_entering_prohibition,
        zone_leaving_prohibition,
        movement_parameters_limitation
    };

    struct MovementLimitation { // move from here to somewhere
        double minCourse;
        double maxCourse;
        double maxSpeed;
    };

    struct Limitation {
        ViolatedType violetedtype;
        std::string feature_id;
        RestrictionType restrictiontype;
        MovementLimitation movementlimitation;
        LimitationType limitationType;
        double distToPoint;
    };

    struct Restrictions {
        std::vector<MultiPolygon> polygons;
        std::vector<Limitation> limitations;
        std::vector<size_t> indexes;
    };

    struct ScenarioVehicle {
        std::string id;
        Vector2 position;
        Vector2 velocity;
        ShipCategory rank;
        double vehicleLength; // ответы на вопросы ТЗ от 24.04.2020
        double distanceDetect; // дистанции обнаружения
    };

    typedef std::vector<ScenarioVehicle> ScenarioState;

    struct ScenarioSettings {
        double minDistanceIntersect; // минимальная дистанция пересечения целью курса корабля(судна) по носу
        double minDistanceDetect; // минимальная дистанция обнаружения
        double visibility;

        double criticalTCPAseconds;
        double criticalCPA;

        std::vector<double> forwardSpeedArray;

        SafeZone safeZone;
    };

    
}
#endif // USV_DEFINES_H