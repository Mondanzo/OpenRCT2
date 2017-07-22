#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "BindingObject.hpp"
#include "Bindings.hpp"

extern "C"
{
    #include "../localisation/localisation.h"
    #include "../ride/ride.h"
}

namespace OpenRCT2 { namespace Scripting { namespace Bindings
{
    class RideBindingObject final : public BindingObject<RideBindingObject>
    {
    public:
        DefineProperty(std::string, "name", NameProperty);
        DefineProperty(sint32, "excitement", ExcitementProperty);
        DefineProperty(sint32, "intensity", IntensityProperty);
        DefineProperty(sint32, "nausea", NauseaProperty);
        DefineProperty(sint32, "totalCustomers", TotalCustomersProperty);

    private:
        uint8       _rideIndex;
        rct_ride *  _ride;

        std::string GetRideName()
        {
            utf8 rideName[128];
            format_string(rideName, sizeof(rideName), _ride->name, &_ride->name_arguments);
            return std::string(rideName);
        }

    protected:
        void SetupPrototype() override
        {
            AddPropertyString<NameProperty>();
            AddPropertyInt32<ExcitementProperty>();
            AddPropertyInt32<IntensityProperty>();
            AddPropertyInt32<NauseaProperty>();
            AddPropertyInt32<TotalCustomersProperty>();

            AddFunction<&RideBindingObject::ToString>("toString");
        }

    public:
        RideBindingObject(duk_context * ctx, uint8 rideIndex, rct_ride * ride)
            : BindingObject("Ride", ctx),
              _rideIndex(rideIndex),
              _ride(ride)
        {
        }

        std::string Get(NameProperty)
        {
            return GetRideName();
        }
        void Set(NameProperty, std::string value)
        {
            ride_set_name(_rideIndex, value.c_str());
        }

        sint32 Get(ExcitementProperty) { return _ride->excitement; }
        void Set(ExcitementProperty, sint32 value) { _ride->excitement = value; }

        sint32 Get(IntensityProperty) { return _ride->intensity; }
        void Set(IntensityProperty, sint32 value) { _ride->intensity = value; }

        sint32 Get(NauseaProperty) { return _ride->nausea; }
        void Set(NauseaProperty, sint32 value) { _ride->nausea = value; }

        sint32 Get(TotalCustomersProperty) { return _ride->total_customers; }
        void Set(TotalCustomersProperty, sint32 value) { _ride->total_customers = value; }

        duk_int_t ToString()
        {
            auto result = "Ride { name: " + GetRideName() + " }";
            duk_push_string(Context, result.c_str());
            return 1;
        }
    };

    void CreateRide(duk_context * ctx, uint8 rideIndex, rct_ride * ride)
    {
        auto rbo = new RideBindingObject(ctx, rideIndex, ride);
        rbo->GetOrCreate("map.rides[" + std::to_string(rideIndex) + "]");
    }
} } }
